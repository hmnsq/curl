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

#جے_بنا_تعریف CURL_DISABLE_FTP

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
  PL_UNIX_گروہ,
  PL_UNIX_ناپ,
  PL_UNIX_ویلا,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } کل_dirsize;

  تعداد {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_نمبر
  } hlinks;

  تعداد {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  تعداد {
    PL_UNIX_گروہ_PRESPACE = 0,
    PL_UNIX_گروہ_NAME
  } گروہ;

  تعداد {
    PL_UNIX_ناپ_PRESPACE = 0,
    PL_UNIX_ناپ_نمبر
  } ناپ;

  تعداد {
    PL_UNIX_ویلا_PREPART1 = 0,
    PL_UNIX_ویلا_PART1,
    PL_UNIX_ویلا_PREPART2,
    PL_UNIX_ویلا_PART2,
    PL_UNIX_ویلا_PREPART3,
    PL_UNIX_ویلا_PART3
  } ویلا;

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
    PL_UNIX_SYMLINK_نشانہ,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

تعريفنوع تعداد {
  PL_WINNT_DATE = 0,
  PL_WINNT_ویلا,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_WINNT_ویلا_PRESPACE = 0,
    PL_WINNT_ویلا_ویلا
  } ویلا;
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
بنية ftp_parselist_انکڑے {
  تعداد {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  اتحاد {
    بنية {
      pl_unix_mainstate مکھ;
      pl_unix_substate sub;
    } UNIX;

    بنية {
      pl_winNT_mainstate مکھ;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  بنية fileinfo *file_انکڑے;
  بنانشانی صحيح شے_لمائی;
  size_t شے_وتھ;
  بنية {
    size_t filename;
    size_t user;
    size_t گروہ;
    size_t ویلا;
    size_t perm;
    size_t symlink_نشانہ;
  } offsets;
};

ساكن فراغ fileinfo_dtor(فراغ *user, فراغ *element)
{
  (فراغ)user;
  Curl_fileinfo_cleanup(element);
}

فراغ Curl_wildcard_init(بنية Wildcard_انکڑے *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

فراغ Curl_wildcard_dtor(بنية Wildcard_انکڑے **wcp)
{
  بنية Wildcard_انکڑے *wc = *wcp;
  إذا(!wc)
    أرجع;

  إذا(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = عدم;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, عدم);
  curlx_safefree(wc->رستہ);
  curlx_safefree(wc->نمونہ);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = عدم;
}

بنية ftp_parselist_انکڑے *Curl_ftp_parselist_انکڑے_alloc(فراغ)
{
  أرجع curlx_calloc(1, حجم(بنية ftp_parselist_انکڑے));
}

فراغ Curl_ftp_parselist_انکڑے_free(بنية ftp_parselist_انکڑے **parserp)
{
  بنية ftp_parselist_انکڑے *parser = *parserp;
  إذا(parser)
    Curl_fileinfo_cleanup(parser->file_انکڑے);
  curlx_free(parser);
  *parserp = عدم;
}

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_انکڑے *pl_انکڑے)
{
  أرجع pl_انکڑے->error;
}

#تعریف_کرو FTP_LP_MALFORMATED_PERM 0x01000000

ساكن بنانشانی صحيح ftp_pl_get_permission(ثابت حرف *str)
{
  بنانشانی صحيح permissions = 0;
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
  /*گروہ*/
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

ساكن CURLcode ftp_pl_داخل_کرو_finfo(بنية Curl_easy *انکڑے,
                                    بنية fileinfo *infop)
{
  curl_fnmatch_callback compare;
  بنية Wildcard_انکڑے *wc = انکڑے->wildcard;
  بنية ftp_wc *ftpwc = wc->ftpwc;
  بنية Curl_llist *llist = &wc->filelist;
  بنية ftp_parselist_انکڑے *parser = ftpwc->parser;
  bool جوڑو = TRUE;
  بنية curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  حرف *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->لڑیاں.گروہ  = parser->offsets.گروہ ?
                          str + parser->offsets.گروہ : عدم;
  finfo->لڑیاں.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : عدم;
  finfo->لڑیاں.نشانہ = parser->offsets.symlink_نشانہ &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_نشانہ : عدم;
  finfo->لڑیاں.ویلا   = str + parser->offsets.ویلا;
  finfo->لڑیاں.user   = parser->offsets.user ?
                          str + parser->offsets.user : عدم;

  /* get correct fnmatch callback */
  compare = انکڑے->سیٹ.fnmatch;
  إذا(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  إذا(strchr(finfo->filename, '/'))
    جوڑو = FALSE;
  وإلا {
    /* a real file name cannot contain a slash */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_شروع(&guard, انکڑے, easy_fnmatch_انکڑے);
    إذا(compare(انکڑے->سیٹ.fnmatch_انکڑے, wc->نمونہ, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      إذا((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->لڑیاں.نشانہ &&
         لڑی_کھوج(finfo->لڑیاں.نشانہ, " -> ")) {
        جوڑو = FALSE;
      }
    }
    وإلا {
      جوڑو = FALSE;
    }
    CURL_CBAPI_سرا(&guard);
  }

  إذا(جوڑو) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  وإلا {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_انکڑے = عدم;
  أرجع CURLE_OK;
}

#تعریف_کرو MAX_FTPLIST_بفر 10000 /* arbitrarily set */

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

ساكن CURLcode parse_unix_totalsize(بنية ftp_parselist_انکڑے *parser,
                                     بنية fileinfo *infop,
                                     ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.کل_dirsize) {
  حالة PL_UNIX_TOTALSIZE_INIT:
    إذا(c == 't') {
      parser->state.UNIX.sub.کل_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->شے_لمائی++;
    }
    وإلا {
      parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    اكسر;
  حالة PL_UNIX_TOTALSIZE_READING:
    parser->شے_لمائی++;
    إذا(c == '\r') {
      parser->شے_لمائی--;
      إذا(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    وإلا إذا(c == '\n') {
      mem[parser->شے_لمائی - 1] = 0;
      إذا(!لڑی_مقابلہ("total ", mem, 6)) {
        ثابت حرف *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        طالما(ISDIGIT(*endptr))
          endptr++;
        إذا(*endptr) {
          أرجع CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_permission(بنية ftp_parselist_انکڑے *parser,
                                      بنية fileinfo *infop,
                                      ثابت حرف c)
{
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  parser->شے_لمائی++;
  إذا((parser->شے_لمائی <= 9) && !strchr("rwx-tTsS", c))
    أرجع CURLE_FTP_BAD_FILE_LIST;

  وإلا إذا(parser->شے_لمائی == 10) {
    بنانشانی صحيح perm;
    إذا(c != ' ')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->شے_وتھ);
    إذا(perm & FTP_LP_MALFORMATED_PERM)
      أرجع CURLE_FTP_BAD_FILE_LIST;

    parser->file_انکڑے->info.flags |= CURLFINFOFLAG_معلوم_PERM;
    parser->file_انکڑے->info.perm = perm;
    parser->offsets.perm = parser->شے_وتھ;

    parser->شے_لمائی = 0;
    parser->state.UNIX.مکھ = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_hlinks(بنية ftp_parselist_انکڑے *parser,
                                  بنية fileinfo *infop,
                                  ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);

  بدل(parser->state.UNIX.sub.hlinks) {
  حالة PL_UNIX_HLINKS_PRESPACE:
    إذا(c != ' ') {
      إذا(ISDIGIT(c) && len) {
        parser->شے_وتھ = len - 1;
        parser->شے_لمائی = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_نمبر;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_HLINKS_نمبر:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      ثابت حرف *p = &mem[parser->شے_وتھ];
      curl_off_t hlinks;
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;

      إذا(!curlx_str_نمبر(&p, &hlinks, LONG_MAX)) {
        parser->file_انکڑے->info.flags |= CURLFINFOFLAG_معلوم_HLINKCOUNT;
        parser->file_انکڑے->info.hardlinks = (لما)hlinks;
      }
      parser->شے_لمائی = 0;
      parser->شے_وتھ = 0;
      parser->state.UNIX.مکھ = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    وإلا إذا(!ISDIGIT(c))
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_user(بنية ftp_parselist_انکڑے *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.user) {
  حالة PL_UNIX_USER_PRESPACE:
    إذا(c != ' ' && len) {
      parser->شے_وتھ = len - 1;
      parser->شے_لمائی = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    اكسر;
  حالة PL_UNIX_USER_PARSING:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.user = parser->شے_وتھ;
      parser->state.UNIX.مکھ = PL_UNIX_گروہ;
      parser->state.UNIX.sub.گروہ = PL_UNIX_گروہ_PRESPACE;
      parser->شے_وتھ = 0;
      parser->شے_لمائی = 0;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_گروہ(بنية ftp_parselist_انکڑے *parser,
                                 بنية fileinfo *infop,
                                 ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.گروہ) {
  حالة PL_UNIX_گروہ_PRESPACE:
    إذا(c != ' ' && len) {
      parser->شے_وتھ = len - 1;
      parser->شے_لمائی = 1;
      parser->state.UNIX.sub.گروہ = PL_UNIX_گروہ_NAME;
    }
    اكسر;
  حالة PL_UNIX_گروہ_NAME:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.گروہ = parser->شے_وتھ;
      parser->state.UNIX.مکھ = PL_UNIX_ناپ;
      parser->state.UNIX.sub.ناپ = PL_UNIX_ناپ_PRESPACE;
      parser->شے_وتھ = 0;
      parser->شے_لمائی = 0;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_ناپ(بنية ftp_parselist_انکڑے *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.ناپ) {
  حالة PL_UNIX_ناپ_PRESPACE:
    إذا(c != ' ') {
      إذا(ISDIGIT(c) && len) {
        parser->شے_وتھ = len - 1;
        parser->شے_لمائی = 1;
        parser->state.UNIX.sub.ناپ = PL_UNIX_ناپ_نمبر;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_ناپ_نمبر:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      ثابت حرف *p = mem + parser->شے_وتھ;
      curl_off_t fsize;
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      إذا(!curlx_str_numblanks(&p, &fsize)) {
        إذا(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_انکڑے->info.flags |= CURLFINFOFLAG_معلوم_ناپ;
          parser->file_انکڑے->info.ناپ = fsize;
        }
        parser->شے_لمائی = 0;
        parser->شے_وتھ = 0;
        parser->state.UNIX.مکھ = PL_UNIX_ویلا;
        parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PREPART1;
      }
    }
    وإلا إذا(!ISDIGIT(c))
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_ویلا(بنية ftp_parselist_انکڑے *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بنية curl_fileinfo *finfo = &infop->info;

  بدل(parser->state.UNIX.sub.ویلا) {
  حالة PL_UNIX_ویلا_PREPART1:
    إذا(c != ' ') {
      إذا(ISALNUM(c) && len) {
        parser->شے_وتھ = len - 1;
        parser->شے_لمائی = 1;
        parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PART1;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_ویلا_PART1:
    parser->شے_لمائی++;
    إذا(c == ' ')
      parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PREPART2;

    وإلا إذا(!ISALNUM(c) && c != '.')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_ویلا_PREPART2:
    parser->شے_لمائی++;
    إذا(c != ' ') {
      إذا(ISALNUM(c))
        parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PART2;
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_ویلا_PART2:
    parser->شے_لمائی++;
    إذا(c == ' ')
      parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PREPART3;
    وإلا إذا(!ISALNUM(c) && c != '.')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_UNIX_ویلا_PREPART3:
    parser->شے_لمائی++;
    إذا(c != ' ') {
      إذا(ISALNUM(c))
        parser->state.UNIX.sub.ویلا = PL_UNIX_ویلا_PART3;
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_ویلا_PART3:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.ویلا = parser->شے_وتھ;
      إذا(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.مکھ = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      وإلا {
        parser->state.UNIX.مکھ = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    وإلا إذا(!ISALNUM(c) && c != '.' && c != ':')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode parse_unix_filename(بنية Curl_easy *انکڑے,
                                    بنية ftp_parselist_انکڑے *parser,
                                    بنية fileinfo *infop,
                                    ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  بدل(parser->state.UNIX.sub.filename) {
  حالة PL_UNIX_FILENAME_PRESPACE:
    إذا(c != ' ' && len) {
      parser->شے_وتھ = len - 1;
      parser->شے_لمائی = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    اكسر;
  حالة PL_UNIX_FILENAME_NAME:
    parser->شے_لمائی++;
    إذا(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    وإلا إذا(c == '\n') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.filename = parser->شے_وتھ;
      parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
      نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
    }
    اكسر;
  حالة PL_UNIX_FILENAME_WINDOWSEOL:
    إذا(c == '\n') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.filename = parser->شے_وتھ;
      parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
      نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
    }
    وإلا
      نتیجہ = CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode parse_unix_symlink(بنية Curl_easy *انکڑے,
                                   بنية ftp_parselist_انکڑے *parser,
                                   بنية fileinfo *infop,
                                   ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  بدل(parser->state.UNIX.sub.symlink) {
  حالة PL_UNIX_SYMLINK_PRESPACE:
    إذا(c != ' ' && len) {
      parser->شے_وتھ = len - 1;
      parser->شے_لمائی = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_NAME:
    parser->شے_لمائی++;
    إذا(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET1:
    parser->شے_لمائی++;
    إذا(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET2:
    parser->شے_لمائی++;
    إذا(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET3:
    parser->شے_لمائی++;
    إذا(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->شے_وتھ + parser->شے_لمائی - 4] = 0;
      parser->offsets.filename = parser->شے_وتھ;
      parser->شے_لمائی = 0;
      parser->شے_وتھ = 0;
    }
    وإلا إذا(c == '\r' || c == '\n')
      أرجع CURLE_FTP_BAD_FILE_LIST;
    وإلا
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET4:
    إذا(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_نشانہ;
      parser->شے_وتھ = len - 1;
      parser->شے_لمائی = 1;
    }
    وإلا
      أرجع CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_نشانہ:
    parser->شے_لمائی++;
    إذا(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    وإلا إذا(c == '\n') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.symlink_نشانہ = parser->شے_وتھ;
      نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
      إذا(نتیجہ)
        اكسر;

      parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_WINDOWSEOL:
    إذا(c == '\n') {
      mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
      parser->offsets.symlink_نشانہ = parser->شے_وتھ;
      نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
      إذا(نتیجہ)
        اكسر;

      parser->state.UNIX.مکھ = PL_UNIX_FILETYPE;
    }
    وإلا
      نتیجہ = CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode parse_unix(بنية Curl_easy *انکڑے,
                           بنية ftp_parselist_انکڑے *parser,
                           بنية fileinfo *infop,
                           ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  CURLcode نتیجہ = CURLE_OK;

  بدل(parser->state.UNIX.مکھ) {
  حالة PL_UNIX_TOTALSIZE:
    نتیجہ = parse_unix_totalsize(parser, infop, c);
    إذا(نتیجہ)
      اكسر;
    إذا(parser->state.UNIX.مکھ != PL_UNIX_FILETYPE)
      اكسر;
    FALLTHROUGH();
  حالة PL_UNIX_FILETYPE:
    نتیجہ = unix_filetype(c, &finfo->filetype);
    إذا(!نتیجہ) {
      parser->state.UNIX.مکھ = PL_UNIX_PERMISSION;
      parser->شے_لمائی = 0;
      parser->شے_وتھ = 1;
    }
    اكسر;
  حالة PL_UNIX_PERMISSION:
    نتیجہ = parse_unix_permission(parser, infop, c);
    اكسر;
  حالة PL_UNIX_HLINKS:
    نتیجہ = parse_unix_hlinks(parser, infop, c);
    اكسر;
  حالة PL_UNIX_USER:
    نتیجہ = parse_unix_user(parser, infop, c);
    اكسر;
  حالة PL_UNIX_گروہ:
    نتیجہ = parse_unix_گروہ(parser, infop, c);
    اكسر;
  حالة PL_UNIX_ناپ:
    نتیجہ = parse_unix_ناپ(parser, infop, c);
    اكسر;
  حالة PL_UNIX_ویلا:
    نتیجہ = parse_unix_ویلا(parser, infop, c);
    اكسر;
  حالة PL_UNIX_FILENAME:
    نتیجہ = parse_unix_filename(انکڑے, parser, infop, c);
    اكسر;
  حالة PL_UNIX_SYMLINK:
    نتیجہ = parse_unix_symlink(انکڑے, parser, infop, c);
    اكسر;
  }
  أرجع نتیجہ;
}

ساكن CURLcode parse_winnt(بنية Curl_easy *انکڑے,
                            بنية ftp_parselist_انکڑے *parser,
                            بنية fileinfo *infop,
                            ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode نتیجہ = CURLE_OK;

  بدل(parser->state.NT.مکھ) {
  حالة PL_WINNT_DATE:
    parser->شے_لمائی++;
    إذا(parser->شے_لمائی < 9) {
      إذا(!strchr("0123456789-", c)) { /* only simple control */
        أرجع CURLE_FTP_BAD_FILE_LIST;
      }
    }
    وإلا إذا(parser->شے_لمائی == 9) {
      إذا(c == ' ') {
        parser->state.NT.مکھ = PL_WINNT_ویلا;
        parser->state.NT.sub.ویلا = PL_WINNT_ویلا_PRESPACE;
      }
      وإلا
        أرجع CURLE_FTP_BAD_FILE_LIST;
    }
    وإلا
      أرجع CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_WINNT_ویلا:
    parser->شے_لمائی++;
    بدل(parser->state.NT.sub.ویلا) {
    حالة PL_WINNT_ویلا_PRESPACE:
      إذا(!ISBLANK(c))
        parser->state.NT.sub.ویلا = PL_WINNT_ویلا_ویلا;
      اكسر;
    حالة PL_WINNT_ویلا_ویلا:
      إذا(c == ' ') {
        parser->offsets.ویلا = parser->شے_وتھ;
        mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
        parser->state.NT.مکھ = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->شے_لمائی = 0;
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
        parser->شے_وتھ = len - 1;
        parser->شے_لمائی = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_DIRORSIZE_CONTENT:
      parser->شے_لمائی++;
      إذا(c == ' ') {
        mem[parser->شے_وتھ + parser->شے_لمائی - 1] = 0;
        إذا(!strcmp("<DIR>", mem + parser->شے_وتھ)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->ناپ = 0;
        }
        وإلا {
          ثابت حرف *p = mem + parser->شے_وتھ;
          إذا(curlx_str_numblanks(&p, &finfo->ناپ)) {
            أرجع CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_انکڑے->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_انکڑے->info.flags |= CURLFINFOFLAG_معلوم_ناپ;
        parser->شے_لمائی = 0;
        parser->state.NT.مکھ = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    }
    اكسر;
  حالة PL_WINNT_FILENAME:
    بدل(parser->state.NT.sub.filename) {
    حالة PL_WINNT_FILENAME_PRESPACE:
      إذا(c != ' ' && len) {
        parser->شے_وتھ = len - 1;
        parser->شے_لمائی = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_CONTENT:
      parser->شے_لمائی++;
      إذا(!len)
        أرجع CURLE_FTP_BAD_FILE_LIST;
      إذا(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      وإلا إذا(c == '\n') {
        parser->offsets.filename = parser->شے_وتھ;
        mem[len - 1] = 0;
        نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
        إذا(نتیجہ)
          أرجع نتیجہ;

        parser->state.NT.مکھ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_WINEOL:
      إذا(c == '\n') {
        parser->offsets.filename = parser->شے_وتھ;
        نتیجہ = ftp_pl_داخل_کرو_finfo(انکڑے, infop);
        إذا(نتیجہ)
          أرجع نتیجہ;

        parser->state.NT.مکھ = PL_WINNT_DATE;
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
bool Curl_ftp_listmode(بنية Curl_easy *انکڑے)
{
  إذا(انکڑے->state.wildcardmatch) {
    بنية Wildcard_انکڑے * ثابت wildcard = انکڑے->wildcard;
    أرجع wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  أرجع FALSE;
}

size_t Curl_ftp_parselist(حرف *بفر, size_t ناپ, size_t nmemb,
                          فراغ *connptr)
{
  size_t bufflen = ناپ * nmemb;
  بنية Curl_easy *انکڑے = (بنية Curl_easy *)connptr;
  بنية ftp_wc *ftpwc = انکڑے->wildcard->ftpwc;
  بنية ftp_parselist_انکڑے *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode نتیجہ;
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
    إذا(!parser->file_انکڑے) { /* tmp file data is not allocated yet */
      parser->file_انکڑے = Curl_fileinfo_alloc();
      إذا(!parser->file_انکڑے) {
        parser->error = CURLE_باہرلا_دا_حافظہ;
        اذهب fail;
      }
      parser->شے_وتھ = 0;
      parser->شے_لمائی = 0;
      curlx_dyn_init(&parser->file_انکڑے->buf, MAX_FTPLIST_بفر);
    }

    infop = parser->file_انکڑے;

    إذا(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_باہرلا_دا_حافظہ;
      اذهب fail;
    }

    بدل(parser->os_type) {
    حالة OS_TYPE_UNIX:
      نتیجہ = parse_unix(انکڑے, parser, infop, c);
      اكسر;
    حالة OS_TYPE_WIN_NT:
      نتیجہ = parse_winnt(انکڑے, parser, infop, c);
      اكسر;
    افتراضي:
      retsize = bufflen + 1;
      اذهب fail;
    }
    إذا(نتیجہ) {
      parser->error = نتیجہ;
      اذهب fail;
    }

    i++;
  }
  أرجع retsize;

fail:

  /* Clean up any allocated memory. */
  إذا(parser->file_انکڑے) {
    Curl_fileinfo_cleanup(parser->file_انکڑے);
    parser->file_انکڑے = عدم;
  }

  أرجع retsize;
}

#جے_انت /* !CURL_DISABLE_FTP */
