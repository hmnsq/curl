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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP

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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileinfo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftplistparser.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fnmatch.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_ਸਮੂਹ,
  PL_UNIX_ਮਾਪ,
  PL_UNIX_ਸਮਾਂ,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
  ਗਿਣਤੀ {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } ਕੁੱਲ_dirsize;

  ਗਿਣਤੀ {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_ਸੰਖਿਆ
  } hlinks;

  ਗਿਣਤੀ {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ਗਿਣਤੀ {
    PL_UNIX_ਸਮੂਹ_PRESPACE = 0,
    PL_UNIX_ਸਮੂਹ_NAME
  } ਸਮੂਹ;

  ਗਿਣਤੀ {
    PL_UNIX_ਮਾਪ_PRESPACE = 0,
    PL_UNIX_ਮਾਪ_ਸੰਖਿਆ
  } ਮਾਪ;

  ਗਿਣਤੀ {
    PL_UNIX_ਸਮਾਂ_PREPART1 = 0,
    PL_UNIX_ਸਮਾਂ_PART1,
    PL_UNIX_ਸਮਾਂ_PREPART2,
    PL_UNIX_ਸਮਾਂ_PART2,
    PL_UNIX_ਸਮਾਂ_PREPART3,
    PL_UNIX_ਸਮਾਂ_PART3
  } ਸਮਾਂ;

  ਗਿਣਤੀ {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ਗਿਣਤੀ {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_ਨਿਸ਼ਾਨਾ,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  PL_WINNT_DATE = 0,
  PL_WINNT_ਸਮਾਂ,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
  ਗਿਣਤੀ {
    PL_WINNT_ਸਮਾਂ_PRESPACE = 0,
    PL_WINNT_ਸਮਾਂ_ਸਮਾਂ
  } ਸਮਾਂ;
  ਗਿਣਤੀ {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ਗਿਣਤੀ {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ {
  ਗਿਣਤੀ {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  ਸੰਘ {
    ਬਣਤਰ {
      pl_unix_mainstate ਮੁੱਖ;
      pl_unix_substate sub;
    } UNIX;

    ਬਣਤਰ {
      pl_winNT_mainstate ਮੁੱਖ;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ਬਣਤਰ fileinfo *file_ਅੰਕੜੇ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਸਤੂ_ਲੰਮਾਈ;
  size_t ਵਸਤੂ_ਵਿੱਥ;
  ਬਣਤਰ {
    size_t filename;
    size_t user;
    size_t ਸਮੂਹ;
    size_t ਸਮਾਂ;
    size_t perm;
    size_t symlink_ਨਿਸ਼ਾਨਾ;
  } offsets;
};

ਸਥਿਰ ਖਾਲੀ fileinfo_dtor(ਖਾਲੀ *user, ਖਾਲੀ *element)
{
  (ਖਾਲੀ)user;
  Curl_fileinfo_cleanup(element);
}

ਖਾਲੀ Curl_wildcard_init(ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

ਖਾਲੀ Curl_wildcard_dtor(ਬਣਤਰ Wildcard_ਅੰਕੜੇ **wcp)
{
  ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc = *wcp;
  ਜੇ(!wc)
    ਵਾਪਸ;

  ਜੇ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ਨਿਰਾ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ਨਿਰਾ);
  curlx_safefree(wc->ਰਸਤਾ);
  curlx_safefree(wc->ਨਮੂਨਾ);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ਨਿਰਾ;
}

ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *Curl_ftp_parselist_ਅੰਕੜੇ_alloc(ਖਾਲੀ)
{
  ਵਾਪਸ curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ));
}

ਖਾਲੀ Curl_ftp_parselist_ਅੰਕੜੇ_free(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ **parserp)
{
  ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser = *parserp;
  ਜੇ(parser)
    Curl_fileinfo_cleanup(parser->file_ਅੰਕੜੇ);
  curlx_free(parser);
  *parserp = ਨਿਰਾ;
}

CURLcode Curl_ftp_parselist_geterror(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *pl_ਅੰਕੜੇ)
{
  ਵਾਪਸ pl_ਅੰਕੜੇ->error;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FTP_LP_MALFORMATED_PERM 0x01000000

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ftp_pl_get_permission(ਅਚਲ ਅੱਖਰ *str)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ permissions = 0;
  /* USER */
  ਜੇ(str[0] == 'r')
    permissions |= 1 << 8;
  ਨਹੀਂਤਾਂ ਜੇ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ਜੇ(str[1] == 'w')
    permissions |= 1 << 7;
  ਨਹੀਂਤਾਂ ਜੇ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ਜੇ(str[2] == 'x')
    permissions |= 1 << 6;
  ਨਹੀਂਤਾਂ ਜੇ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ਨਹੀਂਤਾਂ ਜੇ(str[2] == 'S')
    permissions |= 1 << 11;
  ਨਹੀਂਤਾਂ ਜੇ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*ਸਮੂਹ*/
  ਜੇ(str[3] == 'r')
    permissions |= 1 << 5;
  ਨਹੀਂਤਾਂ ਜੇ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ਜੇ(str[4] == 'w')
    permissions |= 1 << 4;
  ਨਹੀਂਤਾਂ ਜੇ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ਜੇ(str[5] == 'x')
    permissions |= 1 << 3;
  ਨਹੀਂਤਾਂ ਜੇ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ਨਹੀਂਤਾਂ ਜੇ(str[5] == 'S')
    permissions |= 1 << 10;
  ਨਹੀਂਤਾਂ ਜੇ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ਜੇ(str[6] == 'r')
    permissions |= 1 << 2;
  ਨਹੀਂਤਾਂ ਜੇ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ਜੇ(str[7] == 'w')
    permissions |= 1 << 1;
  ਨਹੀਂਤਾਂ ਜੇ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ਜੇ(str[8] == 'x')
    permissions |= 1;
  ਨਹੀਂਤਾਂ ਜੇ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ਨਹੀਂਤਾਂ ਜੇ(str[8] == 'T')
    permissions |= 1 << 9;
  ਨਹੀਂਤਾਂ ਜੇ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ਵਾਪਸ permissions;
}

ਸਥਿਰ CURLcode ftp_pl_ਪਾਓ_finfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ਬਣਤਰ Wildcard_ਅੰਕੜੇ *wc = ਅੰਕੜੇ->wildcard;
  ਬਣਤਰ ftp_wc *ftpwc = wc->ftpwc;
  ਬਣਤਰ Curl_llist *llist = &wc->filelist;
  ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser = ftpwc->parser;
  bool ਜੋੜੋ = TRUE;
  ਬਣਤਰ curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  ਅੱਖਰ *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->ਲੜੀਆਂ.ਸਮੂਹ  = parser->offsets.ਸਮੂਹ ?
                          str + parser->offsets.ਸਮੂਹ : ਨਿਰਾ;
  finfo->ਲੜੀਆਂ.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ਨਿਰਾ;
  finfo->ਲੜੀਆਂ.ਨਿਸ਼ਾਨਾ = parser->offsets.symlink_ਨਿਸ਼ਾਨਾ &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_ਨਿਸ਼ਾਨਾ : ਨਿਰਾ;
  finfo->ਲੜੀਆਂ.ਸਮਾਂ   = str + parser->offsets.ਸਮਾਂ;
  finfo->ਲੜੀਆਂ.user   = parser->offsets.user ?
                          str + parser->offsets.user : ਨਿਰਾ;

  /* get correct fnmatch callback */
  compare = ਅੰਕੜੇ->ਸੈੱਟ.fnmatch;
  ਜੇ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ਜੇ(strchr(finfo->filename, '/'))
    ਜੋੜੋ = FALSE;
  ਨਹੀਂਤਾਂ {
    /* a real file name cannot contain a slash */
    ਬਣਤਰ Curl_mapi_guard guard;
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fnmatch_ਅੰਕੜੇ);
    ਜੇ(compare(ਅੰਕੜੇ->ਸੈੱਟ.fnmatch_ਅੰਕੜੇ, wc->ਨਮੂਨਾ, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ਜੇ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->ਲੜੀਆਂ.ਨਿਸ਼ਾਨਾ &&
         ਲੜੀ_ਖੋਜ(finfo->ਲੜੀਆਂ.ਨਿਸ਼ਾਨਾ, " -> ")) {
        ਜੋੜੋ = FALSE;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜੋੜੋ = FALSE;
    }
    CURL_CBAPI_ਸਿਰਾ(&guard);
  }

  ਜੇ(ਜੋੜੋ) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ਨਹੀਂਤਾਂ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_ਅੰਕੜੇ = ਨਿਰਾ;
  ਵਾਪਸ CURLE_OK;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FTPLIST_ਬਫਰ 10000 /* arbitrarily set */

ਸਥਿਰ CURLcode unix_filetype(ਅਚਲ ਅੱਖਰ c, curlfiletype *t)
{
  ਚੋਣ(c) {
  ਹਾਲਤ '-':
    *t = CURLFILETYPE_FILE;
    ਰੋਕੋ;
  ਹਾਲਤ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ਰੋਕੋ;
  ਹਾਲਤ 'l':
    *t = CURLFILETYPE_SYMLINK;
    ਰੋਕੋ;
  ਹਾਲਤ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ਰੋਕੋ;
  ਹਾਲਤ 's':
    *t = CURLFILETYPE_SOCKET;
    ਰੋਕੋ;
  ਹਾਲਤ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ਰੋਕੋ;
  ਹਾਲਤ 'b':
    *t = CURLFILETYPE_DEVICE_ਖੰਡ;
    ਰੋਕੋ;
  ਹਾਲਤ 'D':
    *t = CURLFILETYPE_DOOR;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_totalsize(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                     ਬਣਤਰ fileinfo *infop,
                                     ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  ਚੋਣ(parser->state.UNIX.sub.ਕੁੱਲ_dirsize) {
  ਹਾਲਤ PL_UNIX_TOTALSIZE_INIT:
    ਜੇ(c == 't') {
      parser->state.UNIX.sub.ਕੁੱਲ_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    }
    ਨਹੀਂਤਾਂ {
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_TOTALSIZE_READING:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == '\r') {
      parser->ਵਸਤੂ_ਲੰਮਾਈ--;
      ਜੇ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == '\n') {
      mem[parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      ਜੇ(!ਲੜੀ_ਤੁਲਨਾ("total ", mem, 6)) {
        ਅਚਲ ਅੱਖਰ *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ਜਦੋਂ(ISDIGIT(*endptr))
          endptr++;
        ਜੇ(*endptr) {
          ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_permission(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                      ਬਣਤਰ fileinfo *infop,
                                      ਅਚਲ ਅੱਖਰ c)
{
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  parser->ਵਸਤੂ_ਲੰਮਾਈ++;
  ਜੇ((parser->ਵਸਤੂ_ਲੰਮਾਈ <= 9) && !strchr("rwx-tTsS", c))
    ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

  ਨਹੀਂਤਾਂ ਜੇ(parser->ਵਸਤੂ_ਲੰਮਾਈ == 10) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ perm;
    ਜੇ(c != ' ')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->ਵਸਤੂ_ਵਿੱਥ);
    ਜੇ(perm & FTP_LP_MALFORMATED_PERM)
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    parser->file_ਅੰਕੜੇ->info.flags |= CURLFINFOFLAG_ਗਿਆਤ_PERM;
    parser->file_ਅੰਕੜੇ->info.perm = perm;
    parser->offsets.perm = parser->ਵਸਤੂ_ਵਿੱਥ;

    parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
    parser->state.UNIX.ਮੁੱਖ = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_hlinks(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                  ਬਣਤਰ fileinfo *infop,
                                  ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);

  ਚੋਣ(parser->state.UNIX.sub.hlinks) {
  ਹਾਲਤ PL_UNIX_HLINKS_PRESPACE:
    ਜੇ(c != ' ') {
      ਜੇ(ISDIGIT(c) && len) {
        parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_ਸੰਖਿਆ;
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_HLINKS_ਸੰਖਿਆ:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      ਅਚਲ ਅੱਖਰ *p = &mem[parser->ਵਸਤੂ_ਵਿੱਥ];
      curl_off_t hlinks;
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;

      ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &hlinks, LONG_MAX)) {
        parser->file_ਅੰਕੜੇ->info.flags |= CURLFINFOFLAG_ਗਿਆਤ_HLINKCOUNT;
        parser->file_ਅੰਕੜੇ->info.hardlinks = (ਲੰਮਾ)hlinks;
      }
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
      parser->ਵਸਤੂ_ਵਿੱਥ = 0;
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ISDIGIT(c))
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_user(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                ਬਣਤਰ fileinfo *infop,
                                ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  ਚੋਣ(parser->state.UNIX.sub.user) {
  ਹਾਲਤ PL_UNIX_USER_PRESPACE:
    ਜੇ(c != ' ' && len) {
      parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_USER_PARSING:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.user = parser->ਵਸਤੂ_ਵਿੱਥ;
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_ਸਮੂਹ;
      parser->state.UNIX.sub.ਸਮੂਹ = PL_UNIX_ਸਮੂਹ_PRESPACE;
      parser->ਵਸਤੂ_ਵਿੱਥ = 0;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_ਸਮੂਹ(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                 ਬਣਤਰ fileinfo *infop,
                                 ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  ਚੋਣ(parser->state.UNIX.sub.ਸਮੂਹ) {
  ਹਾਲਤ PL_UNIX_ਸਮੂਹ_PRESPACE:
    ਜੇ(c != ' ' && len) {
      parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
      parser->state.UNIX.sub.ਸਮੂਹ = PL_UNIX_ਸਮੂਹ_NAME;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮੂਹ_NAME:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.ਸਮੂਹ = parser->ਵਸਤੂ_ਵਿੱਥ;
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_ਮਾਪ;
      parser->state.UNIX.sub.ਮਾਪ = PL_UNIX_ਮਾਪ_PRESPACE;
      parser->ਵਸਤੂ_ਵਿੱਥ = 0;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_ਮਾਪ(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                ਬਣਤਰ fileinfo *infop,
                                ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  ਚੋਣ(parser->state.UNIX.sub.ਮਾਪ) {
  ਹਾਲਤ PL_UNIX_ਮਾਪ_PRESPACE:
    ਜੇ(c != ' ') {
      ਜੇ(ISDIGIT(c) && len) {
        parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
        parser->state.UNIX.sub.ਮਾਪ = PL_UNIX_ਮਾਪ_ਸੰਖਿਆ;
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਮਾਪ_ਸੰਖਿਆ:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      ਅਚਲ ਅੱਖਰ *p = mem + parser->ਵਸਤੂ_ਵਿੱਥ;
      curl_off_t fsize;
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      ਜੇ(!curlx_str_numblanks(&p, &fsize)) {
        ਜੇ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_ਅੰਕੜੇ->info.flags |= CURLFINFOFLAG_ਗਿਆਤ_ਮਾਪ;
          parser->file_ਅੰਕੜੇ->info.ਮਾਪ = fsize;
        }
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
        parser->ਵਸਤੂ_ਵਿੱਥ = 0;
        parser->state.UNIX.ਮੁੱਖ = PL_UNIX_ਸਮਾਂ;
        parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PREPART1;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ISDIGIT(c))
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_ਸਮਾਂ(ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                ਬਣਤਰ fileinfo *infop,
                                ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  ਬਣਤਰ curl_fileinfo *finfo = &infop->info;

  ਚੋਣ(parser->state.UNIX.sub.ਸਮਾਂ) {
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PREPART1:
    ਜੇ(c != ' ') {
      ਜੇ(ISALNUM(c) && len) {
        parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
        parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PART1;
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PART1:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ')
      parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PREPART2;

    ਨਹੀਂਤਾਂ ਜੇ(!ISALNUM(c) && c != '.')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PREPART2:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c != ' ') {
      ਜੇ(ISALNUM(c))
        parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PART2;
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PART2:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ')
      parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PREPART3;
    ਨਹੀਂਤਾਂ ਜੇ(!ISALNUM(c) && c != '.')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PREPART3:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c != ' ') {
      ਜੇ(ISALNUM(c))
        parser->state.UNIX.sub.ਸਮਾਂ = PL_UNIX_ਸਮਾਂ_PART3;
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ_PART3:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.ਸਮਾਂ = parser->ਵਸਤੂ_ਵਿੱਥ;
      ਜੇ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.ਮੁੱਖ = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ਨਹੀਂਤਾਂ {
        parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(!ISALNUM(c) && c != '.' && c != ':')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode parse_unix_filename(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                    ਬਣਤਰ fileinfo *infop,
                                    ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(parser->state.UNIX.sub.filename) {
  ਹਾਲਤ PL_UNIX_FILENAME_PRESPACE:
    ਜੇ(c != ' ' && len) {
      parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_FILENAME_NAME:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ਨਹੀਂਤਾਂ ਜੇ(c == '\n') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.filename = parser->ਵਸਤੂ_ਵਿੱਥ;
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
      ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_FILENAME_WINDOWSEOL:
    ਜੇ(c == '\n') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.filename = parser->ਵਸਤੂ_ਵਿੱਥ;
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
      ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_FTP_BAD_FILE_LIST;
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_unix_symlink(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                                   ਬਣਤਰ fileinfo *infop,
                                   ਅਚਲ ਅੱਖਰ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(parser->state.UNIX.sub.symlink) {
  ਹਾਲਤ PL_UNIX_SYMLINK_PRESPACE:
    ਜੇ(c != ' ' && len) {
      parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_NAME:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ਨਹੀਂਤਾਂ ਜੇ(c == '\r' || c == '\n')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_PRETARGET1:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ਨਹੀਂਤਾਂ ਜੇ(c == '\r' || c == '\n')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਨਹੀਂਤਾਂ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_PRETARGET2:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ਨਹੀਂਤਾਂ ਜੇ(c == '\r' || c == '\n')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਨਹੀਂਤਾਂ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_PRETARGET3:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 4] = 0;
      parser->offsets.filename = parser->ਵਸਤੂ_ਵਿੱਥ;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
      parser->ਵਸਤੂ_ਵਿੱਥ = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ(c == '\r' || c == '\n')
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਨਹੀਂਤਾਂ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_PRETARGET4:
    ਜੇ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_ਨਿਸ਼ਾਨਾ;
      parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_ਨਿਸ਼ਾਨਾ:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ਨਹੀਂਤਾਂ ਜੇ(c == '\n') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.symlink_ਨਿਸ਼ਾਨਾ = parser->ਵਸਤੂ_ਵਿੱਥ;
      ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;

      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK_WINDOWSEOL:
    ਜੇ(c == '\n') {
      mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
      parser->offsets.symlink_ਨਿਸ਼ਾਨਾ = parser->ਵਸਤੂ_ਵਿੱਥ;
      ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;

      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_FILETYPE;
    }
    ਨਹੀਂਤਾਂ
      ਨਤੀਜਾ = CURLE_FTP_BAD_FILE_LIST;

    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_unix(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                           ਬਣਤਰ fileinfo *infop,
                           ਅਚਲ ਅੱਖਰ c)
{
  ਬਣਤਰ curl_fileinfo *finfo = &infop->info;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(parser->state.UNIX.ਮੁੱਖ) {
  ਹਾਲਤ PL_UNIX_TOTALSIZE:
    ਨਤੀਜਾ = parse_unix_totalsize(parser, infop, c);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
    ਜੇ(parser->state.UNIX.ਮੁੱਖ != PL_UNIX_FILETYPE)
      ਰੋਕੋ;
    FALLTHROUGH();
  ਹਾਲਤ PL_UNIX_FILETYPE:
    ਨਤੀਜਾ = unix_filetype(c, &finfo->filetype);
    ਜੇ(!ਨਤੀਜਾ) {
      parser->state.UNIX.ਮੁੱਖ = PL_UNIX_PERMISSION;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
      parser->ਵਸਤੂ_ਵਿੱਥ = 1;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_PERMISSION:
    ਨਤੀਜਾ = parse_unix_permission(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_HLINKS:
    ਨਤੀਜਾ = parse_unix_hlinks(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_USER:
    ਨਤੀਜਾ = parse_unix_user(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮੂਹ:
    ਨਤੀਜਾ = parse_unix_ਸਮੂਹ(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਮਾਪ:
    ਨਤੀਜਾ = parse_unix_ਮਾਪ(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_ਸਮਾਂ:
    ਨਤੀਜਾ = parse_unix_ਸਮਾਂ(parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_FILENAME:
    ਨਤੀਜਾ = parse_unix_filename(ਅੰਕੜੇ, parser, infop, c);
    ਰੋਕੋ;
  ਹਾਲਤ PL_UNIX_SYMLINK:
    ਨਤੀਜਾ = parse_unix_symlink(ਅੰਕੜੇ, parser, infop, c);
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode parse_winnt(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser,
                            ਬਣਤਰ fileinfo *infop,
                            ਅਚਲ ਅੱਖਰ c)
{
  ਬਣਤਰ curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  ਅੱਖਰ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਚੋਣ(parser->state.NT.ਮੁੱਖ) {
  ਹਾਲਤ PL_WINNT_DATE:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਜੇ(parser->ਵਸਤੂ_ਲੰਮਾਈ < 9) {
      ਜੇ(!strchr("0123456789-", c)) { /* only simple control */
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(parser->ਵਸਤੂ_ਲੰਮਾਈ == 9) {
      ਜੇ(c == ' ') {
        parser->state.NT.ਮੁੱਖ = PL_WINNT_ਸਮਾਂ;
        parser->state.NT.sub.ਸਮਾਂ = PL_WINNT_ਸਮਾਂ_PRESPACE;
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
    ਰੋਕੋ;
  ਹਾਲਤ PL_WINNT_ਸਮਾਂ:
    parser->ਵਸਤੂ_ਲੰਮਾਈ++;
    ਚੋਣ(parser->state.NT.sub.ਸਮਾਂ) {
    ਹਾਲਤ PL_WINNT_ਸਮਾਂ_PRESPACE:
      ਜੇ(!ISBLANK(c))
        parser->state.NT.sub.ਸਮਾਂ = PL_WINNT_ਸਮਾਂ_ਸਮਾਂ;
      ਰੋਕੋ;
    ਹਾਲਤ PL_WINNT_ਸਮਾਂ_ਸਮਾਂ:
      ਜੇ(c == ' ') {
        parser->offsets.ਸਮਾਂ = parser->ਵਸਤੂ_ਵਿੱਥ;
        mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
        parser->state.NT.ਮੁੱਖ = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!strchr("APM0123456789:", c))
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_WINNT_DIRORSIZE:
    ਚੋਣ(parser->state.NT.sub.dirorsize) {
    ਹਾਲਤ PL_WINNT_DIRORSIZE_PRESPACE:
      ਜੇ(c != ' ' && len) {
        parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ਰੋਕੋ;
    ਹਾਲਤ PL_WINNT_DIRORSIZE_CONTENT:
      parser->ਵਸਤੂ_ਲੰਮਾਈ++;
      ਜੇ(c == ' ') {
        mem[parser->ਵਸਤੂ_ਵਿੱਥ + parser->ਵਸਤੂ_ਲੰਮਾਈ - 1] = 0;
        ਜੇ(!strcmp("<DIR>", mem + parser->ਵਸਤੂ_ਵਿੱਥ)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->ਮਾਪ = 0;
        }
        ਨਹੀਂਤਾਂ {
          ਅਚਲ ਅੱਖਰ *p = mem + parser->ਵਸਤੂ_ਵਿੱਥ;
          ਜੇ(curlx_str_numblanks(&p, &finfo->ਮਾਪ)) {
            ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_ਅੰਕੜੇ->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_ਅੰਕੜੇ->info.flags |= CURLFINFOFLAG_ਗਿਆਤ_ਮਾਪ;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
        parser->state.NT.ਮੁੱਖ = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  ਹਾਲਤ PL_WINNT_FILENAME:
    ਚੋਣ(parser->state.NT.sub.filename) {
    ਹਾਲਤ PL_WINNT_FILENAME_PRESPACE:
      ਜੇ(c != ' ' && len) {
        parser->ਵਸਤੂ_ਵਿੱਥ = len - 1;
        parser->ਵਸਤੂ_ਲੰਮਾਈ = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ਰੋਕੋ;
    ਹਾਲਤ PL_WINNT_FILENAME_CONTENT:
      parser->ਵਸਤੂ_ਲੰਮਾਈ++;
      ਜੇ(!len)
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;
      ਜੇ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == '\n') {
        parser->offsets.filename = parser->ਵਸਤੂ_ਵਿੱਥ;
        mem[len - 1] = 0;
        ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        parser->state.NT.ਮੁੱਖ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ਰੋਕੋ;
    ਹਾਲਤ PL_WINNT_FILENAME_WINEOL:
      ਜੇ(c == '\n') {
        parser->offsets.filename = parser->ਵਸਤੂ_ਵਿੱਥ;
        ਨਤੀਜਾ = ftp_pl_ਪਾਓ_finfo(ਅੰਕੜੇ, infop);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;

        parser->state.NT.ਮੁੱਖ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_FTP_BAD_FILE_LIST;

      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  }

  ਵਾਪਸ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(ਅੰਕੜੇ->state.wildcardmatch) {
    ਬਣਤਰ Wildcard_ਅੰਕੜੇ * ਅਚਲ wildcard = ਅੰਕੜੇ->wildcard;
    ਵਾਪਸ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ਵਾਪਸ FALSE;
}

size_t Curl_ftp_parselist(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ, size_t nmemb,
                          ਖਾਲੀ *connptr)
{
  size_t bufflen = ਮਾਪ * nmemb;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = (ਬਣਤਰ Curl_easy *)connptr;
  ਬਣਤਰ ftp_wc *ftpwc = ਅੰਕੜੇ->wildcard->ftpwc;
  ਬਣਤਰ ftp_parselist_ਅੰਕੜੇ *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode ਨਤੀਜਾ;
  size_t retsize = bufflen;

  ਜੇ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    ਜਾਓ fail;
  }

  ਜੇ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(ਬਫਰ[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ਜਦੋਂ(i < bufflen) { /* FSM */
    ਅੱਖਰ c = ਬਫਰ[i];
    ਬਣਤਰ fileinfo *infop;
    ਜੇ(!parser->file_ਅੰਕੜੇ) { /* tmp file data is not allocated yet */
      parser->file_ਅੰਕੜੇ = Curl_fileinfo_alloc();
      ਜੇ(!parser->file_ਅੰਕੜੇ) {
        parser->error = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ fail;
      }
      parser->ਵਸਤੂ_ਵਿੱਥ = 0;
      parser->ਵਸਤੂ_ਲੰਮਾਈ = 0;
      curlx_dyn_init(&parser->file_ਅੰਕੜੇ->buf, MAX_FTPLIST_ਬਫਰ);
    }

    infop = parser->file_ਅੰਕੜੇ;

    ਜੇ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ fail;
    }

    ਚੋਣ(parser->os_type) {
    ਹਾਲਤ OS_TYPE_UNIX:
      ਨਤੀਜਾ = parse_unix(ਅੰਕੜੇ, parser, infop, c);
      ਰੋਕੋ;
    ਹਾਲਤ OS_TYPE_WIN_NT:
      ਨਤੀਜਾ = parse_winnt(ਅੰਕੜੇ, parser, infop, c);
      ਰੋਕੋ;
    ਮੂਲ:
      retsize = bufflen + 1;
      ਜਾਓ fail;
    }
    ਜੇ(ਨਤੀਜਾ) {
      parser->error = ਨਤੀਜਾ;
      ਜਾਓ fail;
    }

    i++;
  }
  ਵਾਪਸ retsize;

fail:

  /* Clean up any allocated memory. */
  ਜੇ(parser->file_ਅੰਕੜੇ) {
    Curl_fileinfo_cleanup(parser->file_ਅੰਕੜੇ);
    parser->file_ਅੰਕੜੇ = ਨਿਰਾ;
  }

  ਵਾਪਸ retsize;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_FTP */
