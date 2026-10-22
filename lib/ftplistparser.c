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

#که_نه_تعریف_شوی CURL_DISABLE_FTP

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

#ضمن "urldata.h"
#ضمن "fileinfo.h"
#ضمن "llist.h"
#ضمن "ftp.h"
#ضمن "ftp-int.h"
#ضمن "ftplistparser.h"
#ضمن "curl_fnmatch.h"
#ضمن "multiif.h"
#ضمن "curlx/strparse.h"

تعريفنوع تعداد {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_ډله,
  PL_UNIX_اندازه,
  PL_UNIX_وخت,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } ټول_dirsize;

  تعداد {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_شمېره
  } hlinks;

  تعداد {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  تعداد {
    PL_UNIX_ډله_PRESPACE = 0,
    PL_UNIX_ډله_NAME
  } ډله;

  تعداد {
    PL_UNIX_اندازه_PRESPACE = 0,
    PL_UNIX_اندازه_شمېره
  } اندازه;

  تعداد {
    PL_UNIX_وخت_PREPART1 = 0,
    PL_UNIX_وخت_PART1,
    PL_UNIX_وخت_PREPART2,
    PL_UNIX_وخت_PART2,
    PL_UNIX_وخت_PREPART3,
    PL_UNIX_وخت_PART3
  } وخت;

  تعداد {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  تعداد {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_موخه,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

تعريفنوع تعداد {
  PL_WINNT_DATE = 0,
  PL_WINNT_وخت,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_WINNT_وخت_PRESPACE = 0,
    PL_WINNT_وخت_وخت
  } وخت;
  تعداد {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  تعداد {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
بنية ftp_parselist_معلومات {
  تعداد {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  اتحاد {
    بنية {
      pl_unix_mainstate اصلي;
      pl_unix_substate sub;
    } UNIX;

    بنية {
      pl_winNT_mainstate اصلي;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  بنية fileinfo *file_معلومات;
  بینښه صحيح توکی_اوږدتیا;
  size_t توکی_دننه_واټن;
  بنية {
    size_t filename;
    size_t user;
    size_t ډله;
    size_t وخت;
    size_t perm;
    size_t symlink_موخه;
  } offsets;
};

ساكن فراغ fileinfo_dtor(فراغ *user, فراغ *element)
{
  (فراغ)user;
  Curl_fileinfo_cleanup(element);
}

فراغ Curl_wildcard_init(بنية Wildcard_معلومات *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

فراغ Curl_wildcard_dtor(بنية Wildcard_معلومات **wcp)
{
  بنية Wildcard_معلومات *wc = *wcp;
  إذا(!wc)
    أرجع;

  إذا(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = عدم;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, عدم);
  curlx_safefree(wc->لار);
  curlx_safefree(wc->نمونه);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = عدم;
}

بنية ftp_parselist_معلومات *Curl_ftp_parselist_معلومات_alloc(فراغ)
{
  أرجع curlx_calloc(1, حجم(بنية ftp_parselist_معلومات));
}

فراغ Curl_ftp_parselist_معلومات_free(بنية ftp_parselist_معلومات **parserp)
{
  بنية ftp_parselist_معلومات *parser = *parserp;
  إذا(parser)
    Curl_fileinfo_cleanup(parser->file_معلومات);
  curlx_free(parser);
  *parserp = عدم;
}

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_معلومات *pl_معلومات)
{
  أرجع pl_معلومات->error;
}

#تعریف_کړه FTP_LP_MALFORMATED_PERM 0x01000000

ساكن بینښه صحيح ftp_pl_get_permission(ثابت حرف *str)
{
  بینښه صحيح permissions = 0;
  /* USER */
  إذا(str[0] == 'r')
    permissions |= 1 << 8;
  وإلا إذا(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  إذا(str[1] == 'w')
    permissions |= 1 << 7;
  وإلا إذا(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  إذا(str[2] == 'x')
    permissions |= 1 << 6;
  وإلا إذا(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  وإلا إذا(str[2] == 'S')
    permissions |= 1 << 11;
  وإلا إذا(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*ډله*/
  إذا(str[3] == 'r')
    permissions |= 1 << 5;
  وإلا إذا(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  إذا(str[4] == 'w')
    permissions |= 1 << 4;
  وإلا إذا(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  إذا(str[5] == 'x')
    permissions |= 1 << 3;
  وإلا إذا(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  وإلا إذا(str[5] == 'S')
    permissions |= 1 << 10;
  وإلا إذا(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  إذا(str[6] == 'r')
    permissions |= 1 << 2;
  وإلا إذا(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  إذا(str[7] == 'w')
    permissions |= 1 << 1;
  وإلا إذا(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  إذا(str[8] == 'x')
    permissions |= 1;
  وإلا إذا(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  وإلا إذا(str[8] == 'T')
    permissions |= 1 << 9;
  وإلا إذا(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  أرجع permissions;
}

ساكن CURLcode ftp_pl_ورننباسه_finfo(بنية Curl_easy *معلومات,
                                    بنية fileinfo *infop)
{
  curl_fnmatch_callback compare;
  بنية Wildcard_معلومات *wc = معلومات->wildcard;
  بنية ftp_wc *ftpwc = wc->ftpwc;
  بنية Curl_llist *llist = &wc->filelist;
  بنية ftp_parselist_معلومات *parser = ftpwc->parser;
  bool جمع_کړه = TRUE;
  بنية curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  حرف *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->مزي_ګانې.ډله  = parser->offsets.ډله ?
                          str + parser->offsets.ډله : عدم;
  finfo->مزي_ګانې.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : عدم;
  finfo->مزي_ګانې.موخه = parser->offsets.symlink_موخه &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_موخه : عدم;
  finfo->مزي_ګانې.وخت   = str + parser->offsets.وخت;
  finfo->مزي_ګانې.user   = parser->offsets.user ?
                          str + parser->offsets.user : عدم;

  /* get correct fnmatch callback */
  compare = معلومات->سېټ.fnmatch;
  إذا(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  إذا(strchr(finfo->filename, '/'))
    جمع_کړه = FALSE;
  وإلا {
    /* a real file name cannot contain a slash */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_پیل(&guard, معلومات, easy_fnmatch_معلومات);
    إذا(compare(معلومات->سېټ.fnmatch_معلومات, wc->نمونه, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      إذا((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->مزي_ګانې.موخه &&
         مزي_پلټنه(finfo->مزي_ګانې.موخه, " -> ")) {
        جمع_کړه = FALSE;
      }
    }
    وإلا {
      جمع_کړه = FALSE;
    }
    CURL_CBAPI_پای_ټکی(&guard);
  }

  إذا(جمع_کړه) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  وإلا {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_معلومات = عدم;
  أرجع CURLE_OK;
}

#تعریف_کړه MAX_FTPLIST_بفر 10000 /* arbitrarily set */

ساكن CURLcode unix_filetype(ثابت حرف c, curlfiletype *t)
{
  بدل(c) {
  حالة '-':
    *t = CURLFILETYPE_FILE;
    اكسر;
  حالة 'd':
    *t = CURLFILETYPE_DIRECTORY;
    اكسر;
  حالة 'l':
    *t = CURLFILETYPE_SYMLINK;
    اكسر;
  حالة 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    اكسر;
  حالة 's':
    *t = CURLFILETYPE_SOCKET;
    اكسر;
  حالة 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    اكسر;
  حالة 'b':
    *t = CURLFILETYPE_DEVICE_بلاک;
    اكسر;
  حالة 'D':
    *t = CURLFILETYPE_DOOR;
    اكسر;
  افتراضي:
    أرجع CURLE_FTP_BAD_FILE_LIST;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_totalsize(بنية ftp_parselist_معلومات *parser,
                                     بنية fileinfo *infop,
                                     ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.ټول_dirsize) {
  حالة PL_UNIX_TOTALSIZE_INIT:
    إذا(c == 't') {
      parser->state.UNIX.sub.ټول_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->توکی_اوږدتیا++;
    }
    وإلا {
      parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    اكسر;
  حالة PL_UNIX_TOTALSIZE_READING:
    parser->توکی_اوږدتیا++;
    إذا(c == '\r') {
      parser->توکی_اوږدتیا--;
      إذا(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    وإلا إذا(c == '\n') {
      mem[parser->توکی_اوږدتیا - 1] = 0;
      إذا(!مزي_پرتله("total ", mem, 6)) {
        ثابت حرف *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        طالما(ISDIGIT(*endptr))
          endptr++;
        إذا(*endptr) {
          أرجع CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_permission(بنية ftp_parselist_معلومات *parser,
                                      بنية fileinfo *infop,
                                      ثابت حرف c)
{
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  parser->توکی_اوږدتیا++;
  إذا((parser->توکی_اوږدتیا <= 9) && !strchr("rwx-tTsS", c))
    أرجع CURLE_FTP_BAD_FILE_LIST;

  وإلا إذا(parser->توکی_اوږدتیا == 10) {
    بینښه صحيح perm;
    إذا(c != ' ')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->توکی_دننه_واټن);
    إذا(perm & FTP_LP_MALFORMATED_PERM)
      أرجع CURLE_FTP_BAD_FILE_LIST;

    parser->file_معلومات->info.flags |= CURLFINFOFLAG_معلوم_PERM;
    parser->file_معلومات->info.perm = perm;
    parser->offsets.perm = parser->توکی_دننه_واټن;

    parser->توکی_اوږدتیا = 0;
    parser->state.UNIX.اصلي = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_hlinks(بنية ftp_parselist_معلومات *parser,
                                  بنية fileinfo *infop,
                                  ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);

  بدل(parser->state.UNIX.sub.hlinks) {
  حالة PL_UNIX_HLINKS_PRESPACE:
    إذا(c != ' ') {
      إذا(ISDIGIT(c) && len) {
        parser->توکی_دننه_واټن = len - 1;
        parser->توکی_اوږدتیا = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_شمېره;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_HLINKS_شمېره:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      ثابت حرف *p = &mem[parser->توکی_دننه_واټن];
      curl_off_t hlinks;
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;

      إذا(!curlx_str_شمېره(&p, &hlinks, LONG_MAX)) {
        parser->file_معلومات->info.flags |= CURLFINFOFLAG_معلوم_HLINKCOUNT;
        parser->file_معلومات->info.hardlinks = (اوږد)hlinks;
      }
      parser->توکی_اوږدتیا = 0;
      parser->توکی_دننه_واټن = 0;
      parser->state.UNIX.اصلي = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    وإلا إذا(!ISDIGIT(c))
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_user(بنية ftp_parselist_معلومات *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.user) {
  حالة PL_UNIX_USER_PRESPACE:
    إذا(c != ' ' && len) {
      parser->توکی_دننه_واټن = len - 1;
      parser->توکی_اوږدتیا = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    اكسر;
  حالة PL_UNIX_USER_PARSING:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.user = parser->توکی_دننه_واټن;
      parser->state.UNIX.اصلي = PL_UNIX_ډله;
      parser->state.UNIX.sub.ډله = PL_UNIX_ډله_PRESPACE;
      parser->توکی_دننه_واټن = 0;
      parser->توکی_اوږدتیا = 0;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_ډله(بنية ftp_parselist_معلومات *parser,
                                 بنية fileinfo *infop,
                                 ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.ډله) {
  حالة PL_UNIX_ډله_PRESPACE:
    إذا(c != ' ' && len) {
      parser->توکی_دننه_واټن = len - 1;
      parser->توکی_اوږدتیا = 1;
      parser->state.UNIX.sub.ډله = PL_UNIX_ډله_NAME;
    }
    اكسر;
  حالة PL_UNIX_ډله_NAME:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.ډله = parser->توکی_دننه_واټن;
      parser->state.UNIX.اصلي = PL_UNIX_اندازه;
      parser->state.UNIX.sub.اندازه = PL_UNIX_اندازه_PRESPACE;
      parser->توکی_دننه_واټن = 0;
      parser->توکی_اوږدتیا = 0;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_اندازه(بنية ftp_parselist_معلومات *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.اندازه) {
  حالة PL_UNIX_اندازه_PRESPACE:
    إذا(c != ' ') {
      إذا(ISDIGIT(c) && len) {
        parser->توکی_دننه_واټن = len - 1;
        parser->توکی_اوږدتیا = 1;
        parser->state.UNIX.sub.اندازه = PL_UNIX_اندازه_شمېره;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_اندازه_شمېره:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      ثابت حرف *p = mem + parser->توکی_دننه_واټن;
      curl_off_t fsize;
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      إذا(!curlx_str_numblanks(&p, &fsize)) {
        إذا(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_معلومات->info.flags |= CURLFINFOFLAG_معلوم_اندازه;
          parser->file_معلومات->info.اندازه = fsize;
        }
        parser->توکی_اوږدتیا = 0;
        parser->توکی_دننه_واټن = 0;
        parser->state.UNIX.اصلي = PL_UNIX_وخت;
        parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PREPART1;
      }
    }
    وإلا إذا(!ISDIGIT(c))
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_وخت(بنية ftp_parselist_معلومات *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بنية curl_fileinfo *finfo = &infop->info;

  بدل(parser->state.UNIX.sub.وخت) {
  حالة PL_UNIX_وخت_PREPART1:
    إذا(c != ' ') {
      إذا(ISALNUM(c) && len) {
        parser->توکی_دننه_واټن = len - 1;
        parser->توکی_اوږدتیا = 1;
        parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PART1;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_وخت_PART1:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ')
      parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PREPART2;

    وإلا إذا(!ISALNUM(c) && c != '.')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_وخت_PREPART2:
    parser->توکی_اوږدتیا++;
    إذا(c != ' ') {
      إذا(ISALNUM(c))
        parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PART2;
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_وخت_PART2:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ')
      parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PREPART3;
    وإلا إذا(!ISALNUM(c) && c != '.')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_UNIX_وخت_PREPART3:
    parser->توکی_اوږدتیا++;
    إذا(c != ' ') {
      إذا(ISALNUM(c))
        parser->state.UNIX.sub.وخت = PL_UNIX_وخت_PART3;
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_وخت_PART3:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.وخت = parser->توکی_دننه_واټن;
      إذا(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.اصلي = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      وإلا {
        parser->state.UNIX.اصلي = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    وإلا إذا(!ISALNUM(c) && c != '.' && c != ':')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_filename(بنية Curl_easy *معلومات,
                                    بنية ftp_parselist_معلومات *parser,
                                    بنية fileinfo *infop,
                                    ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode پایله = CURLE_OK;

  بدل(parser->state.UNIX.sub.filename) {
  حالة PL_UNIX_FILENAME_PRESPACE:
    إذا(c != ' ' && len) {
      parser->توکی_دننه_واټن = len - 1;
      parser->توکی_اوږدتیا = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    اكسر;
  حالة PL_UNIX_FILENAME_NAME:
    parser->توکی_اوږدتیا++;
    إذا(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    وإلا إذا(c == '\n') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.filename = parser->توکی_دننه_واټن;
      parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
      پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
    }
    اكسر;
  حالة PL_UNIX_FILENAME_WINDOWSEOL:
    إذا(c == '\n') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.filename = parser->توکی_دننه_واټن;
      parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
      پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
    }
    وإلا
      پایله = CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode parse_unix_symlink(بنية Curl_easy *معلومات,
                                   بنية ftp_parselist_معلومات *parser,
                                   بنية fileinfo *infop,
                                   ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode پایله = CURLE_OK;

  بدل(parser->state.UNIX.sub.symlink) {
  حالة PL_UNIX_SYMLINK_PRESPACE:
    إذا(c != ' ' && len) {
      parser->توکی_دننه_واټن = len - 1;
      parser->توکی_اوږدتیا = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_NAME:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET1:
    parser->توکی_اوږدتیا++;
    إذا(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET2:
    parser->توکی_اوږدتیا++;
    إذا(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET3:
    parser->توکی_اوږدتیا++;
    إذا(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 4] = 0;
      parser->offsets.filename = parser->توکی_دننه_واټن;
      parser->توکی_اوږدتیا = 0;
      parser->توکی_دننه_واټن = 0;
    }
    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET4:
    إذا(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_موخه;
      parser->توکی_دننه_واټن = len - 1;
      parser->توکی_اوږدتیا = 1;
    }
    وإلا
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_موخه:
    parser->توکی_اوږدتیا++;
    إذا(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    وإلا إذا(c == '\n') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.symlink_موخه = parser->توکی_دننه_واټن;
      پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
      إذا(پایله)
        اكسر;

      parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_WINDOWSEOL:
    إذا(c == '\n') {
      mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
      parser->offsets.symlink_موخه = parser->توکی_دننه_واټن;
      پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
      إذا(پایله)
        اكسر;

      parser->state.UNIX.اصلي = PL_UNIX_FILETYPE;
    }
    وإلا
      پایله = CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode parse_unix(بنية Curl_easy *معلومات,
                           بنية ftp_parselist_معلومات *parser,
                           بنية fileinfo *infop,
                           ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  CURLcode پایله = CURLE_OK;

  بدل(parser->state.UNIX.اصلي) {
  حالة PL_UNIX_TOTALSIZE:
    پایله = parse_unix_totalsize(parser, infop, c);
    إذا(پایله)
      اكسر;
    إذا(parser->state.UNIX.اصلي != PL_UNIX_FILETYPE)
      اكسر;
    FALLTHROUGH();
  حالة PL_UNIX_FILETYPE:
    پایله = unix_filetype(c, &finfo->filetype);
    إذا(!پایله) {
      parser->state.UNIX.اصلي = PL_UNIX_PERMISSION;
      parser->توکی_اوږدتیا = 0;
      parser->توکی_دننه_واټن = 1;
    }
    اكسر;
  حالة PL_UNIX_PERMISSION:
    پایله = parse_unix_permission(parser, infop, c);
    اكسر;
  حالة PL_UNIX_HLINKS:
    پایله = parse_unix_hlinks(parser, infop, c);
    اكسر;
  حالة PL_UNIX_USER:
    پایله = parse_unix_user(parser, infop, c);
    اكسر;
  حالة PL_UNIX_ډله:
    پایله = parse_unix_ډله(parser, infop, c);
    اكسر;
  حالة PL_UNIX_اندازه:
    پایله = parse_unix_اندازه(parser, infop, c);
    اكسر;
  حالة PL_UNIX_وخت:
    پایله = parse_unix_وخت(parser, infop, c);
    اكسر;
  حالة PL_UNIX_FILENAME:
    پایله = parse_unix_filename(معلومات, parser, infop, c);
    اكسر;
  حالة PL_UNIX_SYMLINK:
    پایله = parse_unix_symlink(معلومات, parser, infop, c);
    اكسر;
  }
  أرجع پایله;
}

ساكن CURLcode parse_winnt(بنية Curl_easy *معلومات,
                            بنية ftp_parselist_معلومات *parser,
                            بنية fileinfo *infop,
                            ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode پایله = CURLE_OK;

  بدل(parser->state.NT.اصلي) {
  حالة PL_WINNT_DATE:
    parser->توکی_اوږدتیا++;
    إذا(parser->توکی_اوږدتیا < 9) {
      إذا(!strchr("0123456789-", c)) { /* only simple control */
        أرجع CURLE_FTP_BAD_FILE_LIST;
      }
    }
    وإلا إذا(parser->توکی_اوږدتیا == 9) {
      إذا(c == ' ') {
        parser->state.NT.اصلي = PL_WINNT_وخت;
        parser->state.NT.sub.وخت = PL_WINNT_وخت_PRESPACE;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    وإلا
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_WINNT_وخت:
    parser->توکی_اوږدتیا++;
    بدل(parser->state.NT.sub.وخت) {
    حالة PL_WINNT_وخت_PRESPACE:
      إذا(!ISBLANK(c))
        parser->state.NT.sub.وخت = PL_WINNT_وخت_وخت;
      اكسر;
    حالة PL_WINNT_وخت_وخت:
      إذا(c == ' ') {
        parser->offsets.وخت = parser->توکی_دننه_واټن;
        mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
        parser->state.NT.اصلي = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->توکی_اوږدتیا = 0;
      }
      وإلا إذا(!strchr("APM0123456789:", c))
        أرجع CURLE_FTP_BAD_FILE_LIST;
      اكسر;
    }
    اكسر;
  حالة PL_WINNT_DIRORSIZE:
    بدل(parser->state.NT.sub.dirorsize) {
    حالة PL_WINNT_DIRORSIZE_PRESPACE:
      إذا(c != ' ' && len) {
        parser->توکی_دننه_واټن = len - 1;
        parser->توکی_اوږدتیا = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_DIRORSIZE_CONTENT:
      parser->توکی_اوږدتیا++;
      إذا(c == ' ') {
        mem[parser->توکی_دننه_واټن + parser->توکی_اوږدتیا - 1] = 0;
        إذا(!strcmp("<DIR>", mem + parser->توکی_دننه_واټن)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->اندازه = 0;
        }
        وإلا {
          ثابت حرف *p = mem + parser->توکی_دننه_واټن;
          إذا(curlx_str_numblanks(&p, &finfo->اندازه)) {
            أرجع CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_معلومات->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_معلومات->info.flags |= CURLFINFOFLAG_معلوم_اندازه;
        parser->توکی_اوږدتیا = 0;
        parser->state.NT.اصلي = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    }
    اكسر;
  حالة PL_WINNT_FILENAME:
    بدل(parser->state.NT.sub.filename) {
    حالة PL_WINNT_FILENAME_PRESPACE:
      إذا(c != ' ' && len) {
        parser->توکی_دننه_واټن = len - 1;
        parser->توکی_اوږدتیا = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_CONTENT:
      parser->توکی_اوږدتیا++;
      إذا(!len)
        أرجع CURLE_FTP_BAD_FILE_LIST;
      إذا(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      وإلا إذا(c == '\n') {
        parser->offsets.filename = parser->توکی_دننه_واټن;
        mem[len - 1] = 0;
        پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
        إذا(پایله)
          أرجع پایله;

        parser->state.NT.اصلي = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_WINEOL:
      إذا(c == '\n') {
        parser->offsets.filename = parser->توکی_دننه_واټن;
        پایله = ftp_pl_ورننباسه_finfo(معلومات, infop);
        إذا(پایله)
          أرجع پایله;

        parser->state.NT.اصلي = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;

      اكسر;
    }
    اكسر;
  }

  أرجع CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(بنية Curl_easy *معلومات)
{
  إذا(معلومات->state.wildcardmatch) {
    بنية Wildcard_معلومات * ثابت wildcard = معلومات->wildcard;
    أرجع wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  أرجع FALSE;
}

size_t Curl_ftp_parselist(حرف *بفر, size_t اندازه, size_t nmemb,
                          فراغ *connptr)
{
  size_t bufflen = اندازه * nmemb;
  بنية Curl_easy *معلومات = (بنية Curl_easy *)connptr;
  بنية ftp_wc *ftpwc = معلومات->wildcard->ftpwc;
  بنية ftp_parselist_معلومات *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode پایله;
  size_t retsize = bufflen;

  إذا(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    اذهب fail;
  }

  إذا(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(بفر[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  طالما(i < bufflen) { /* FSM */
    حرف c = بفر[i];
    بنية fileinfo *infop;
    إذا(!parser->file_معلومات) { /* tmp file data is not allocated yet */
      parser->file_معلومات = Curl_fileinfo_alloc();
      إذا(!parser->file_معلومات) {
        parser->error = CURLE_بهر_د_حافظه;
        اذهب fail;
      }
      parser->توکی_دننه_واټن = 0;
      parser->توکی_اوږدتیا = 0;
      curlx_dyn_init(&parser->file_معلومات->buf, MAX_FTPLIST_بفر);
    }

    infop = parser->file_معلومات;

    إذا(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_بهر_د_حافظه;
      اذهب fail;
    }

    بدل(parser->os_type) {
    حالة OS_TYPE_UNIX:
      پایله = parse_unix(معلومات, parser, infop, c);
      اكسر;
    حالة OS_TYPE_WIN_NT:
      پایله = parse_winnt(معلومات, parser, infop, c);
      اكسر;
    افتراضي:
      retsize = bufflen + 1;
      اذهب fail;
    }
    إذا(پایله) {
      parser->error = پایله;
      اذهب fail;
    }

    i++;
  }
  أرجع retsize;

fail:

  /* Clean up any allocated memory. */
  إذا(parser->file_معلومات) {
    Curl_fileinfo_cleanup(parser->file_معلومات);
    parser->file_معلومات = عدم;
  }

  أرجع retsize;
}

#د_که_پای /* !CURL_DISABLE_FTP */
