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

#اگر_غیر_تعریف_شدہ CURL_DISABLE_FTP

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

#شامل "urldata.h"
#شامل "fileinfo.h"
#شامل "llist.h"
#شامل "ftp.h"
#شامل "ftp-int.h"
#شامل "ftplistparser.h"
#شامل "curl_fnmatch.h"
#شامل "multiif.h"
#شامل "curlx/strparse.h"

قسمتعریف شمار {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_گروہ,
  PL_UNIX_سائز,
  PL_UNIX_وقت,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

قسمتعریف اتحاد {
  شمار {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } کل_dirsize;

  شمار {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_نمبر
  } hlinks;

  شمار {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  شمار {
    PL_UNIX_گروہ_PRESPACE = 0,
    PL_UNIX_گروہ_NAME
  } گروہ;

  شمار {
    PL_UNIX_سائز_PRESPACE = 0,
    PL_UNIX_سائز_نمبر
  } سائز;

  شمار {
    PL_UNIX_وقت_PREPART1 = 0,
    PL_UNIX_وقت_PART1,
    PL_UNIX_وقت_PREPART2,
    PL_UNIX_وقت_PART2,
    PL_UNIX_وقت_PREPART3,
    PL_UNIX_وقت_PART3
  } وقت;

  شمار {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  شمار {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_ہدف,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

قسمتعریف شمار {
  PL_WINNT_DATE = 0,
  PL_WINNT_وقت,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

قسمتعریف اتحاد {
  شمار {
    PL_WINNT_وقت_PRESPACE = 0,
    PL_WINNT_وقت_وقت
  } وقت;
  شمار {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  شمار {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ساخت ftp_parselist_ڈیٹا {
  شمار {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  اتحاد {
    ساخت {
      pl_unix_mainstate مین;
      pl_unix_substate sub;
    } UNIX;

    ساخت {
      pl_winNT_mainstate مین;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ساخت fileinfo *file_ڈیٹا;
  غیرعلامتی صحیح شے_لمبائی;
  size_t شے_انحراف;
  ساخت {
    size_t filename;
    size_t user;
    size_t گروہ;
    size_t وقت;
    size_t perm;
    size_t symlink_ہدف;
  } offsets;
};

ساکن خالی fileinfo_dtor(خالی *user, خالی *element)
{
  (خالی)user;
  Curl_fileinfo_cleanup(element);
}

خالی Curl_wildcard_init(ساخت Wildcard_ڈیٹا *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

خالی Curl_wildcard_dtor(ساخت Wildcard_ڈیٹا **wcp)
{
  ساخت Wildcard_ڈیٹا *wc = *wcp;
  اگر(!wc)
    واپس;

  اگر(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = تہی;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, تہی);
  curlx_safefree(wc->راستہ);
  curlx_safefree(wc->نمونہ);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = تہی;
}

ساخت ftp_parselist_ڈیٹا *Curl_ftp_parselist_ڈیٹا_alloc(خالی)
{
  واپس curlx_calloc(1, حجم(ساخت ftp_parselist_ڈیٹا));
}

خالی Curl_ftp_parselist_ڈیٹا_free(ساخت ftp_parselist_ڈیٹا **parserp)
{
  ساخت ftp_parselist_ڈیٹا *parser = *parserp;
  اگر(parser)
    Curl_fileinfo_cleanup(parser->file_ڈیٹا);
  curlx_free(parser);
  *parserp = تہی;
}

CURLcode Curl_ftp_parselist_geterror(ساخت ftp_parselist_ڈیٹا *pl_ڈیٹا)
{
  واپس pl_ڈیٹا->error;
}

#تعریف_کرو FTP_LP_MALFORMATED_PERM 0x01000000

ساکن غیرعلامتی صحیح ftp_pl_get_permission(ثابت حرف *str)
{
  غیرعلامتی صحیح permissions = 0;
  /* USER */
  اگر(str[0] == 'r')
    permissions |= 1 << 8;
  ورنہ اگر(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[1] == 'w')
    permissions |= 1 << 7;
  ورنہ اگر(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  اگر(str[2] == 'x')
    permissions |= 1 << 6;
  ورنہ اگر(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ورنہ اگر(str[2] == 'S')
    permissions |= 1 << 11;
  ورنہ اگر(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*گروہ*/
  اگر(str[3] == 'r')
    permissions |= 1 << 5;
  ورنہ اگر(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[4] == 'w')
    permissions |= 1 << 4;
  ورنہ اگر(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[5] == 'x')
    permissions |= 1 << 3;
  ورنہ اگر(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ورنہ اگر(str[5] == 'S')
    permissions |= 1 << 10;
  ورنہ اگر(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  اگر(str[6] == 'r')
    permissions |= 1 << 2;
  ورنہ اگر(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[7] == 'w')
    permissions |= 1 << 1;
  ورنہ اگر(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[8] == 'x')
    permissions |= 1;
  ورنہ اگر(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ورنہ اگر(str[8] == 'T')
    permissions |= 1 << 9;
  ورنہ اگر(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  واپس permissions;
}

ساکن CURLcode ftp_pl_داخل_کرو_finfo(ساخت Curl_easy *ڈیٹا,
                                    ساخت fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ساخت Wildcard_ڈیٹا *wc = ڈیٹا->wildcard;
  ساخت ftp_wc *ftpwc = wc->ftpwc;
  ساخت Curl_llist *llist = &wc->filelist;
  ساخت ftp_parselist_ڈیٹا *parser = ftpwc->parser;
  bool جوڑو = TRUE;
  ساخت curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  حرف *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->لڑیاں.گروہ  = parser->offsets.گروہ ?
                          str + parser->offsets.گروہ : تہی;
  finfo->لڑیاں.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : تہی;
  finfo->لڑیاں.ہدف = parser->offsets.symlink_ہدف &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_ہدف : تہی;
  finfo->لڑیاں.وقت   = str + parser->offsets.وقت;
  finfo->لڑیاں.user   = parser->offsets.user ?
                          str + parser->offsets.user : تہی;

  /* get correct fnmatch callback */
  compare = ڈیٹا->سیٹ.fnmatch;
  اگر(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  اگر(strchr(finfo->filename, '/'))
    جوڑو = FALSE;
  ورنہ {
    /* a real file name cannot contain a slash */
    ساخت Curl_mapi_guard guard;
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fnmatch_ڈیٹا);
    اگر(compare(ڈیٹا->سیٹ.fnmatch_ڈیٹا, wc->نمونہ, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      اگر((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->لڑیاں.ہدف &&
         لڑی_تلاش(finfo->لڑیاں.ہدف, " -> ")) {
        جوڑو = FALSE;
      }
    }
    ورنہ {
      جوڑو = FALSE;
    }
    CURL_CBAPI_انتہا(&guard);
  }

  اگر(جوڑو) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ورنہ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_ڈیٹا = تہی;
  واپس CURLE_OK;
}

#تعریف_کرو MAX_FTPLIST_بفر 10000 /* arbitrarily set */

ساکن CURLcode unix_filetype(ثابت حرف c, curlfiletype *t)
{
  انتخاب(c) {
  حالت '-':
    *t = CURLFILETYPE_FILE;
    توڑو;
  حالت 'd':
    *t = CURLFILETYPE_DIRECTORY;
    توڑو;
  حالت 'l':
    *t = CURLFILETYPE_SYMLINK;
    توڑو;
  حالت 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    توڑو;
  حالت 's':
    *t = CURLFILETYPE_SOCKET;
    توڑو;
  حالت 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    توڑو;
  حالت 'b':
    *t = CURLFILETYPE_DEVICE_بلاک;
    توڑو;
  حالت 'D':
    *t = CURLFILETYPE_DOOR;
    توڑو;
  طےشدہ:
    واپس CURLE_FTP_BAD_FILE_LIST;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_totalsize(ساخت ftp_parselist_ڈیٹا *parser,
                                     ساخت fileinfo *infop,
                                     ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.کل_dirsize) {
  حالت PL_UNIX_TOTALSIZE_INIT:
    اگر(c == 't') {
      parser->state.UNIX.sub.کل_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->شے_لمبائی++;
    }
    ورنہ {
      parser->state.UNIX.مین = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    توڑو;
  حالت PL_UNIX_TOTALSIZE_READING:
    parser->شے_لمبائی++;
    اگر(c == '\r') {
      parser->شے_لمبائی--;
      اگر(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ورنہ اگر(c == '\n') {
      mem[parser->شے_لمبائی - 1] = 0;
      اگر(!لڑی_موازنہ("total ", mem, 6)) {
        ثابت حرف *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        جب(ISDIGIT(*endptr))
          endptr++;
        اگر(*endptr) {
          واپس CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.مین = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_permission(ساخت ftp_parselist_ڈیٹا *parser,
                                      ساخت fileinfo *infop,
                                      ثابت حرف c)
{
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  parser->شے_لمبائی++;
  اگر((parser->شے_لمبائی <= 9) && !strchr("rwx-tTsS", c))
    واپس CURLE_FTP_BAD_FILE_LIST;

  ورنہ اگر(parser->شے_لمبائی == 10) {
    غیرعلامتی صحیح perm;
    اگر(c != ' ')
      واپس CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->شے_انحراف);
    اگر(perm & FTP_LP_MALFORMATED_PERM)
      واپس CURLE_FTP_BAD_FILE_LIST;

    parser->file_ڈیٹا->info.flags |= CURLFINFOFLAG_معلوم_PERM;
    parser->file_ڈیٹا->info.perm = perm;
    parser->offsets.perm = parser->شے_انحراف;

    parser->شے_لمبائی = 0;
    parser->state.UNIX.مین = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_hlinks(ساخت ftp_parselist_ڈیٹا *parser,
                                  ساخت fileinfo *infop,
                                  ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);

  انتخاب(parser->state.UNIX.sub.hlinks) {
  حالت PL_UNIX_HLINKS_PRESPACE:
    اگر(c != ' ') {
      اگر(ISDIGIT(c) && len) {
        parser->شے_انحراف = len - 1;
        parser->شے_لمبائی = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_نمبر;
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  حالت PL_UNIX_HLINKS_نمبر:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      ثابت حرف *p = &mem[parser->شے_انحراف];
      curl_off_t hlinks;
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;

      اگر(!curlx_str_نمبر(&p, &hlinks, LONG_MAX)) {
        parser->file_ڈیٹا->info.flags |= CURLFINFOFLAG_معلوم_HLINKCOUNT;
        parser->file_ڈیٹا->info.hardlinks = (طویل)hlinks;
      }
      parser->شے_لمبائی = 0;
      parser->شے_انحراف = 0;
      parser->state.UNIX.مین = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ورنہ اگر(!ISDIGIT(c))
      واپس CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_user(ساخت ftp_parselist_ڈیٹا *parser,
                                ساخت fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.user) {
  حالت PL_UNIX_USER_PRESPACE:
    اگر(c != ' ' && len) {
      parser->شے_انحراف = len - 1;
      parser->شے_لمبائی = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    توڑو;
  حالت PL_UNIX_USER_PARSING:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.user = parser->شے_انحراف;
      parser->state.UNIX.مین = PL_UNIX_گروہ;
      parser->state.UNIX.sub.گروہ = PL_UNIX_گروہ_PRESPACE;
      parser->شے_انحراف = 0;
      parser->شے_لمبائی = 0;
    }
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_گروہ(ساخت ftp_parselist_ڈیٹا *parser,
                                 ساخت fileinfo *infop,
                                 ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.گروہ) {
  حالت PL_UNIX_گروہ_PRESPACE:
    اگر(c != ' ' && len) {
      parser->شے_انحراف = len - 1;
      parser->شے_لمبائی = 1;
      parser->state.UNIX.sub.گروہ = PL_UNIX_گروہ_NAME;
    }
    توڑو;
  حالت PL_UNIX_گروہ_NAME:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.گروہ = parser->شے_انحراف;
      parser->state.UNIX.مین = PL_UNIX_سائز;
      parser->state.UNIX.sub.سائز = PL_UNIX_سائز_PRESPACE;
      parser->شے_انحراف = 0;
      parser->شے_لمبائی = 0;
    }
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_سائز(ساخت ftp_parselist_ڈیٹا *parser,
                                ساخت fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.سائز) {
  حالت PL_UNIX_سائز_PRESPACE:
    اگر(c != ' ') {
      اگر(ISDIGIT(c) && len) {
        parser->شے_انحراف = len - 1;
        parser->شے_لمبائی = 1;
        parser->state.UNIX.sub.سائز = PL_UNIX_سائز_نمبر;
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  حالت PL_UNIX_سائز_نمبر:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      ثابت حرف *p = mem + parser->شے_انحراف;
      curl_off_t fsize;
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      اگر(!curlx_str_numblanks(&p, &fsize)) {
        اگر(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_ڈیٹا->info.flags |= CURLFINFOFLAG_معلوم_سائز;
          parser->file_ڈیٹا->info.سائز = fsize;
        }
        parser->شے_لمبائی = 0;
        parser->شے_انحراف = 0;
        parser->state.UNIX.مین = PL_UNIX_وقت;
        parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PREPART1;
      }
    }
    ورنہ اگر(!ISDIGIT(c))
      واپس CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_وقت(ساخت ftp_parselist_ڈیٹا *parser,
                                ساخت fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  ساخت curl_fileinfo *finfo = &infop->info;

  انتخاب(parser->state.UNIX.sub.وقت) {
  حالت PL_UNIX_وقت_PREPART1:
    اگر(c != ' ') {
      اگر(ISALNUM(c) && len) {
        parser->شے_انحراف = len - 1;
        parser->شے_لمبائی = 1;
        parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PART1;
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  حالت PL_UNIX_وقت_PART1:
    parser->شے_لمبائی++;
    اگر(c == ' ')
      parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PREPART2;

    ورنہ اگر(!ISALNUM(c) && c != '.')
      واپس CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  حالت PL_UNIX_وقت_PREPART2:
    parser->شے_لمبائی++;
    اگر(c != ' ') {
      اگر(ISALNUM(c))
        parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PART2;
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  حالت PL_UNIX_وقت_PART2:
    parser->شے_لمبائی++;
    اگر(c == ' ')
      parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PREPART3;
    ورنہ اگر(!ISALNUM(c) && c != '.')
      واپس CURLE_FTP_BAD_FILE_LIST;
    توڑو;
  حالت PL_UNIX_وقت_PREPART3:
    parser->شے_لمبائی++;
    اگر(c != ' ') {
      اگر(ISALNUM(c))
        parser->state.UNIX.sub.وقت = PL_UNIX_وقت_PART3;
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    توڑو;
  حالت PL_UNIX_وقت_PART3:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.وقت = parser->شے_انحراف;
      اگر(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.مین = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ورنہ {
        parser->state.UNIX.مین = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ورنہ اگر(!ISALNUM(c) && c != '.' && c != ':')
      واپس CURLE_FTP_BAD_FILE_LIST;
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode parse_unix_filename(ساخت Curl_easy *ڈیٹا,
                                    ساخت ftp_parselist_ڈیٹا *parser,
                                    ساخت fileinfo *infop,
                                    ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(parser->state.UNIX.sub.filename) {
  حالت PL_UNIX_FILENAME_PRESPACE:
    اگر(c != ' ' && len) {
      parser->شے_انحراف = len - 1;
      parser->شے_لمبائی = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    توڑو;
  حالت PL_UNIX_FILENAME_NAME:
    parser->شے_لمبائی++;
    اگر(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ورنہ اگر(c == '\n') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.filename = parser->شے_انحراف;
      parser->state.UNIX.مین = PL_UNIX_FILETYPE;
      نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
    }
    توڑو;
  حالت PL_UNIX_FILENAME_WINDOWSEOL:
    اگر(c == '\n') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.filename = parser->شے_انحراف;
      parser->state.UNIX.مین = PL_UNIX_FILETYPE;
      نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
    }
    ورنہ
      نتیجہ = CURLE_FTP_BAD_FILE_LIST;
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode parse_unix_symlink(ساخت Curl_easy *ڈیٹا,
                                   ساخت ftp_parselist_ڈیٹا *parser,
                                   ساخت fileinfo *infop,
                                   ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(parser->state.UNIX.sub.symlink) {
  حالت PL_UNIX_SYMLINK_PRESPACE:
    اگر(c != ' ' && len) {
      parser->شے_انحراف = len - 1;
      parser->شے_لمبائی = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    توڑو;
  حالت PL_UNIX_SYMLINK_NAME:
    parser->شے_لمبائی++;
    اگر(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ورنہ اگر(c == '\r' || c == '\n')
      واپس CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  حالت PL_UNIX_SYMLINK_PRETARGET1:
    parser->شے_لمبائی++;
    اگر(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ورنہ اگر(c == '\r' || c == '\n')
      واپس CURLE_FTP_BAD_FILE_LIST;
    ورنہ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    توڑو;
  حالت PL_UNIX_SYMLINK_PRETARGET2:
    parser->شے_لمبائی++;
    اگر(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ورنہ اگر(c == '\r' || c == '\n')
      واپس CURLE_FTP_BAD_FILE_LIST;
    ورنہ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    توڑو;
  حالت PL_UNIX_SYMLINK_PRETARGET3:
    parser->شے_لمبائی++;
    اگر(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->شے_انحراف + parser->شے_لمبائی - 4] = 0;
      parser->offsets.filename = parser->شے_انحراف;
      parser->شے_لمبائی = 0;
      parser->شے_انحراف = 0;
    }
    ورنہ اگر(c == '\r' || c == '\n')
      واپس CURLE_FTP_BAD_FILE_LIST;
    ورنہ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    توڑو;
  حالت PL_UNIX_SYMLINK_PRETARGET4:
    اگر(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_ہدف;
      parser->شے_انحراف = len - 1;
      parser->شے_لمبائی = 1;
    }
    ورنہ
      واپس CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  حالت PL_UNIX_SYMLINK_ہدف:
    parser->شے_لمبائی++;
    اگر(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ورنہ اگر(c == '\n') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.symlink_ہدف = parser->شے_انحراف;
      نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
      اگر(نتیجہ)
        توڑو;

      parser->state.UNIX.مین = PL_UNIX_FILETYPE;
    }
    توڑو;
  حالت PL_UNIX_SYMLINK_WINDOWSEOL:
    اگر(c == '\n') {
      mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
      parser->offsets.symlink_ہدف = parser->شے_انحراف;
      نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
      اگر(نتیجہ)
        توڑو;

      parser->state.UNIX.مین = PL_UNIX_FILETYPE;
    }
    ورنہ
      نتیجہ = CURLE_FTP_BAD_FILE_LIST;

    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode parse_unix(ساخت Curl_easy *ڈیٹا,
                           ساخت ftp_parselist_ڈیٹا *parser,
                           ساخت fileinfo *infop,
                           ثابت حرف c)
{
  ساخت curl_fileinfo *finfo = &infop->info;
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(parser->state.UNIX.مین) {
  حالت PL_UNIX_TOTALSIZE:
    نتیجہ = parse_unix_totalsize(parser, infop, c);
    اگر(نتیجہ)
      توڑو;
    اگر(parser->state.UNIX.مین != PL_UNIX_FILETYPE)
      توڑو;
    FALLTHROUGH();
  حالت PL_UNIX_FILETYPE:
    نتیجہ = unix_filetype(c, &finfo->filetype);
    اگر(!نتیجہ) {
      parser->state.UNIX.مین = PL_UNIX_PERMISSION;
      parser->شے_لمبائی = 0;
      parser->شے_انحراف = 1;
    }
    توڑو;
  حالت PL_UNIX_PERMISSION:
    نتیجہ = parse_unix_permission(parser, infop, c);
    توڑو;
  حالت PL_UNIX_HLINKS:
    نتیجہ = parse_unix_hlinks(parser, infop, c);
    توڑو;
  حالت PL_UNIX_USER:
    نتیجہ = parse_unix_user(parser, infop, c);
    توڑو;
  حالت PL_UNIX_گروہ:
    نتیجہ = parse_unix_گروہ(parser, infop, c);
    توڑو;
  حالت PL_UNIX_سائز:
    نتیجہ = parse_unix_سائز(parser, infop, c);
    توڑو;
  حالت PL_UNIX_وقت:
    نتیجہ = parse_unix_وقت(parser, infop, c);
    توڑو;
  حالت PL_UNIX_FILENAME:
    نتیجہ = parse_unix_filename(ڈیٹا, parser, infop, c);
    توڑو;
  حالت PL_UNIX_SYMLINK:
    نتیجہ = parse_unix_symlink(ڈیٹا, parser, infop, c);
    توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode parse_winnt(ساخت Curl_easy *ڈیٹا,
                            ساخت ftp_parselist_ڈیٹا *parser,
                            ساخت fileinfo *infop,
                            ثابت حرف c)
{
  ساخت curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  انتخاب(parser->state.NT.مین) {
  حالت PL_WINNT_DATE:
    parser->شے_لمبائی++;
    اگر(parser->شے_لمبائی < 9) {
      اگر(!strchr("0123456789-", c)) { /* only simple control */
        واپس CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ورنہ اگر(parser->شے_لمبائی == 9) {
      اگر(c == ' ') {
        parser->state.NT.مین = PL_WINNT_وقت;
        parser->state.NT.sub.وقت = PL_WINNT_وقت_PRESPACE;
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;
    }
    ورنہ
      واپس CURLE_FTP_BAD_FILE_LIST;
    توڑو;
  حالت PL_WINNT_وقت:
    parser->شے_لمبائی++;
    انتخاب(parser->state.NT.sub.وقت) {
    حالت PL_WINNT_وقت_PRESPACE:
      اگر(!ISBLANK(c))
        parser->state.NT.sub.وقت = PL_WINNT_وقت_وقت;
      توڑو;
    حالت PL_WINNT_وقت_وقت:
      اگر(c == ' ') {
        parser->offsets.وقت = parser->شے_انحراف;
        mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
        parser->state.NT.مین = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->شے_لمبائی = 0;
      }
      ورنہ اگر(!strchr("APM0123456789:", c))
        واپس CURLE_FTP_BAD_FILE_LIST;
      توڑو;
    }
    توڑو;
  حالت PL_WINNT_DIRORSIZE:
    انتخاب(parser->state.NT.sub.dirorsize) {
    حالت PL_WINNT_DIRORSIZE_PRESPACE:
      اگر(c != ' ' && len) {
        parser->شے_انحراف = len - 1;
        parser->شے_لمبائی = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      توڑو;
    حالت PL_WINNT_DIRORSIZE_CONTENT:
      parser->شے_لمبائی++;
      اگر(c == ' ') {
        mem[parser->شے_انحراف + parser->شے_لمبائی - 1] = 0;
        اگر(!strcmp("<DIR>", mem + parser->شے_انحراف)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->سائز = 0;
        }
        ورنہ {
          ثابت حرف *p = mem + parser->شے_انحراف;
          اگر(curlx_str_numblanks(&p, &finfo->سائز)) {
            واپس CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_ڈیٹا->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_ڈیٹا->info.flags |= CURLFINFOFLAG_معلوم_سائز;
        parser->شے_لمبائی = 0;
        parser->state.NT.مین = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      توڑو;
    }
    توڑو;
  حالت PL_WINNT_FILENAME:
    انتخاب(parser->state.NT.sub.filename) {
    حالت PL_WINNT_FILENAME_PRESPACE:
      اگر(c != ' ' && len) {
        parser->شے_انحراف = len - 1;
        parser->شے_لمبائی = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      توڑو;
    حالت PL_WINNT_FILENAME_CONTENT:
      parser->شے_لمبائی++;
      اگر(!len)
        واپس CURLE_FTP_BAD_FILE_LIST;
      اگر(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ورنہ اگر(c == '\n') {
        parser->offsets.filename = parser->شے_انحراف;
        mem[len - 1] = 0;
        نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
        اگر(نتیجہ)
          واپس نتیجہ;

        parser->state.NT.مین = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      توڑو;
    حالت PL_WINNT_FILENAME_WINEOL:
      اگر(c == '\n') {
        parser->offsets.filename = parser->شے_انحراف;
        نتیجہ = ftp_pl_داخل_کرو_finfo(ڈیٹا, infop);
        اگر(نتیجہ)
          واپس نتیجہ;

        parser->state.NT.مین = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ورنہ
        واپس CURLE_FTP_BAD_FILE_LIST;

      توڑو;
    }
    توڑو;
  }

  واپس CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ساخت Curl_easy *ڈیٹا)
{
  اگر(ڈیٹا->state.wildcardmatch) {
    ساخت Wildcard_ڈیٹا * ثابت wildcard = ڈیٹا->wildcard;
    واپس wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  واپس FALSE;
}

size_t Curl_ftp_parselist(حرف *بفر, size_t سائز, size_t nmemb,
                          خالی *connptr)
{
  size_t bufflen = سائز * nmemb;
  ساخت Curl_easy *ڈیٹا = (ساخت Curl_easy *)connptr;
  ساخت ftp_wc *ftpwc = ڈیٹا->wildcard->ftpwc;
  ساخت ftp_parselist_ڈیٹا *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode نتیجہ;
  size_t retsize = bufflen;

  اگر(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    جاؤ fail;
  }

  اگر(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(بفر[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  جب(i < bufflen) { /* FSM */
    حرف c = بفر[i];
    ساخت fileinfo *infop;
    اگر(!parser->file_ڈیٹا) { /* tmp file data is not allocated yet */
      parser->file_ڈیٹا = Curl_fileinfo_alloc();
      اگر(!parser->file_ڈیٹا) {
        parser->error = CURLE_برآمد_کا_حافظہ;
        جاؤ fail;
      }
      parser->شے_انحراف = 0;
      parser->شے_لمبائی = 0;
      curlx_dyn_init(&parser->file_ڈیٹا->buf, MAX_FTPLIST_بفر);
    }

    infop = parser->file_ڈیٹا;

    اگر(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_برآمد_کا_حافظہ;
      جاؤ fail;
    }

    انتخاب(parser->os_type) {
    حالت OS_TYPE_UNIX:
      نتیجہ = parse_unix(ڈیٹا, parser, infop, c);
      توڑو;
    حالت OS_TYPE_WIN_NT:
      نتیجہ = parse_winnt(ڈیٹا, parser, infop, c);
      توڑو;
    طےشدہ:
      retsize = bufflen + 1;
      جاؤ fail;
    }
    اگر(نتیجہ) {
      parser->error = نتیجہ;
      جاؤ fail;
    }

    i++;
  }
  واپس retsize;

fail:

  /* Clean up any allocated memory. */
  اگر(parser->file_ڈیٹا) {
    Curl_fileinfo_cleanup(parser->file_ڈیٹا);
    parser->file_ڈیٹا = تہی;
  }

  واپس retsize;
}

#اگر_ختم /* !CURL_DISABLE_FTP */
