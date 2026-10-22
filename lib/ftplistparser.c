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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_FTP

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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "fileinfo.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "ftp-int.h"
#ಅನ್ತರ್ಭಾವ "ftplistparser.h"
#ಅನ್ತರ್ಭಾವ "curl_fnmatch.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_ಗುಂಪು,
  PL_UNIX_ಗಾತ್ರ,
  PL_UNIX_ಸಮಯ,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
  ಎಣಿಕೆ {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } ಒಟ್ಟು_dirsize;

  ಎಣಿಕೆ {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_ಸಂಖ್ಯೆ
  } hlinks;

  ಎಣಿಕೆ {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ಎಣಿಕೆ {
    PL_UNIX_ಗುಂಪು_PRESPACE = 0,
    PL_UNIX_ಗುಂಪು_NAME
  } ಗುಂಪು;

  ಎಣಿಕೆ {
    PL_UNIX_ಗಾತ್ರ_PRESPACE = 0,
    PL_UNIX_ಗಾತ್ರ_ಸಂಖ್ಯೆ
  } ಗಾತ್ರ_2;

  ಎಣಿಕೆ {
    PL_UNIX_ಸಮಯ_PREPART1 = 0,
    PL_UNIX_ಸಮಯ_PART1,
    PL_UNIX_ಸಮಯ_PREPART2,
    PL_UNIX_ಸಮಯ_PART2,
    PL_UNIX_ಸಮಯ_PREPART3,
    PL_UNIX_ಸಮಯ_PART3
  } ಸಮಯ;

  ಎಣಿಕೆ {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ಎಣಿಕೆ {
    PL_UNIX_SYMLINK_PRESPACE = 0,
    PL_UNIX_SYMLINK_NAME,
    PL_UNIX_SYMLINK_PRETARGET1,
    PL_UNIX_SYMLINK_PRETARGET2,
    PL_UNIX_SYMLINK_PRETARGET3,
    PL_UNIX_SYMLINK_PRETARGET4,
    PL_UNIX_SYMLINK_ಗುರಿ,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  PL_WINNT_DATE = 0,
  PL_WINNT_ಸಮಯ,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
  ಎಣಿಕೆ {
    PL_WINNT_ಸಮಯ_PRESPACE = 0,
    PL_WINNT_ಸಮಯ_ಸಮಯ
  } ಸಮಯ;
  ಎಣಿಕೆ {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ಎಣಿಕೆ {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ {
  ಎಣಿಕೆ {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  ಒಕ್ಕೂಟ {
    ರಚನೆ {
      pl_unix_mainstate ಮುಖ್ಯ;
      pl_unix_substate sub;
    } UNIX;

    ರಚನೆ {
      pl_winNT_mainstate ಮುಖ್ಯ;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ರಚನೆ fileinfo *file_ದತ್ತಾಂಶ;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಂಶ_ಉದ್ದಳತೆ;
  size_t ಅಂಶ_ಸ್ಥಾನಾಂತರ;
  ರಚನೆ {
    size_t filename;
    size_t user;
    size_t ಗುಂಪು;
    size_t ಸಮಯ;
    size_t perm;
    size_t symlink_ಗುರಿ;
  } offsets;
};

ಸ್ಥಿರ ಶೂನ್ಯ fileinfo_dtor(ಶೂನ್ಯ *user, ಶೂನ್ಯ *element)
{
  (ಶೂನ್ಯ)user;
  Curl_fileinfo_cleanup(element);
}

ಶೂನ್ಯ Curl_wildcard_init(ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

ಶೂನ್ಯ Curl_wildcard_dtor(ರಚನೆ Wildcard_ದತ್ತಾಂಶ **wcp)
{
  ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc = *wcp;
  ಆದರೆ(!wc)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ಖಾಲಿ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ಖಾಲಿ);
  curlx_safefree(wc->ಮಾರ್ಗ);
  curlx_safefree(wc->ನಮೂನೆ);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ಖಾಲಿ;
}

ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *Curl_ftp_parselist_ದತ್ತಾಂಶ_alloc(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ));
}

ಶೂನ್ಯ Curl_ftp_parselist_ದತ್ತಾಂಶ_free(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ **parserp)
{
  ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser = *parserp;
  ಆದರೆ(parser)
    Curl_fileinfo_cleanup(parser->file_ದತ್ತಾಂಶ);
  curlx_free(parser);
  *parserp = ಖಾಲಿ;
}

CURLcode Curl_ftp_parselist_geterror(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *pl_ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ pl_ದತ್ತಾಂಶ->error;
}

#ವ್ಯಾಖ್ಯಾನಿಸು FTP_LP_MALFORMATED_PERM 0x01000000

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ftp_pl_get_permission(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ permissions = 0;
  /* USER */
  ಆದರೆ(str[0] == 'r')
    permissions |= 1 << 8;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಆದರೆ(str[1] == 'w')
    permissions |= 1 << 7;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ಆದರೆ(str[2] == 'x')
    permissions |= 1 << 6;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[2] == 'S')
    permissions |= 1 << 11;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /*ಗುಂಪು*/
  ಆದರೆ(str[3] == 'r')
    permissions |= 1 << 5;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಆದರೆ(str[4] == 'w')
    permissions |= 1 << 4;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಆದರೆ(str[5] == 'x')
    permissions |= 1 << 3;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[5] == 'S')
    permissions |= 1 << 10;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ಆದರೆ(str[6] == 'r')
    permissions |= 1 << 2;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಆದರೆ(str[7] == 'w')
    permissions |= 1 << 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಆದರೆ(str[8] == 'x')
    permissions |= 1;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[8] == 'T')
    permissions |= 1 << 9;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ಹಿಂತಿರುಗಿ permissions;
}

ಸ್ಥಿರ CURLcode ftp_pl_ಸೇರಿಸು_finfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ರಚನೆ Wildcard_ದತ್ತಾಂಶ *wc = ದತ್ತಾಂಶ->wildcard;
  ರಚನೆ ftp_wc *ftpwc = wc->ftpwc;
  ರಚನೆ Curl_llist *llist = &wc->filelist;
  ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser = ftpwc->parser;
  bool ಕೂಡಿಸು = TRUE;
  ರಚನೆ curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  ಅಕ್ಷರ *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->ಅಕ್ಷರಸರಣಿಗಳು.ಗುಂಪು  = parser->offsets.ಗುಂಪು ?
                          str + parser->offsets.ಗುಂಪು : ಖಾಲಿ;
  finfo->ಅಕ್ಷರಸರಣಿಗಳು.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ಖಾಲಿ;
  finfo->ಅಕ್ಷರಸರಣಿಗಳು.ಗುರಿ = parser->offsets.symlink_ಗುರಿ &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_ಗುರಿ : ಖಾಲಿ;
  finfo->ಅಕ್ಷರಸರಣಿಗಳು.ಸಮಯ   = str + parser->offsets.ಸಮಯ;
  finfo->ಅಕ್ಷರಸರಣಿಗಳು.user   = parser->offsets.user ?
                          str + parser->offsets.user : ಖಾಲಿ;

  /* get correct fnmatch callback */
  compare = ದತ್ತಾಂಶ->ಗಣ.fnmatch;
  ಆದರೆ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ಆದರೆ(strchr(finfo->filename, '/'))
    ಕೂಡಿಸು = FALSE;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* a real file name cannot contain a slash */
    ರಚನೆ Curl_mapi_guard guard;
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fnmatch_ದತ್ತಾಂಶ);
    ಆದರೆ(compare(ದತ್ತಾಂಶ->ಗಣ.fnmatch_ದತ್ತಾಂಶ, wc->ನಮೂನೆ, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ಆದರೆ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->ಅಕ್ಷರಸರಣಿಗಳು.ಗುರಿ &&
         ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(finfo->ಅಕ್ಷರಸರಣಿಗಳು.ಗುರಿ, " -> ")) {
        ಕೂಡಿಸು = FALSE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಕೂಡಿಸು = FALSE;
    }
    CURL_CBAPI_ಕೊನೆ(&guard);
  }

  ಆದರೆ(ಕೂಡಿಸು) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_ದತ್ತಾಂಶ = ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FTPLIST_ಬಫರ್ 10000 /* arbitrarily set */

ಸ್ಥಿರ CURLcode unix_filetype(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c, curlfiletype *t)
{
  ಆಯ್ಕೆ(c) {
  ಸ್ಥಿತಿ '-':
    *t = CURLFILETYPE_FILE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'l':
    *t = CURLFILETYPE_SYMLINK;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 's':
    *t = CURLFILETYPE_SOCKET;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'b':
    *t = CURLFILETYPE_DEVICE_ಖಂಡ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'D':
    *t = CURLFILETYPE_DOOR;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_totalsize(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                     ರಚನೆ fileinfo *infop,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  ಆಯ್ಕೆ(parser->state.UNIX.sub.ಒಟ್ಟು_dirsize) {
  ಸ್ಥಿತಿ PL_UNIX_TOTALSIZE_INIT:
    ಆದರೆ(c == 't') {
      parser->state.UNIX.sub.ಒಟ್ಟು_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_TOTALSIZE_READING:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == '\r') {
      parser->ಅಂಶ_ಉದ್ದಳತೆ--;
      ಆದರೆ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\n') {
      mem[parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("total ", mem, 6)) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ಆಗ(ISDIGIT(*endptr))
          endptr++;
        ಆದರೆ(*endptr) {
          ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_permission(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                      ರಚನೆ fileinfo *infop,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  parser->ಅಂಶ_ಉದ್ದಳತೆ++;
  ಆದರೆ((parser->ಅಂಶ_ಉದ್ದಳತೆ <= 9) && !strchr("rwx-tTsS", c))
    ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(parser->ಅಂಶ_ಉದ್ದಳತೆ == 10) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ perm;
    ಆದರೆ(c != ' ')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ(perm & FTP_LP_MALFORMATED_PERM)
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    parser->file_ದತ್ತಾಂಶ->info.flags |= CURLFINFOFLAG_ತಿಳಿದ_PERM;
    parser->file_ದತ್ತಾಂಶ->info.perm = perm;
    parser->offsets.perm = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;

    parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
    parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_hlinks(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                  ರಚನೆ fileinfo *infop,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);

  ಆಯ್ಕೆ(parser->state.UNIX.sub.hlinks) {
  ಸ್ಥಿತಿ PL_UNIX_HLINKS_PRESPACE:
    ಆದರೆ(c != ' ') {
      ಆದರೆ(ISDIGIT(c) && len) {
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_ಸಂಖ್ಯೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_HLINKS_ಸಂಖ್ಯೆ:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ];
      curl_off_t hlinks;
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;

      ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &hlinks, LONG_MAX)) {
        parser->file_ದತ್ತಾಂಶ->info.flags |= CURLFINFOFLAG_ತಿಳಿದ_HLINKCOUNT;
        parser->file_ದತ್ತಾಂಶ->info.hardlinks = (ದೀರ್ಘ)hlinks;
      }
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ISDIGIT(c))
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_user(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                ರಚನೆ fileinfo *infop,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  ಆಯ್ಕೆ(parser->state.UNIX.sub.user) {
  ಸ್ಥಿತಿ PL_UNIX_USER_PRESPACE:
    ಆದರೆ(c != ' ' && len) {
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_USER_PARSING:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.user = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_ಗುಂಪು;
      parser->state.UNIX.sub.ಗುಂಪು = PL_UNIX_ಗುಂಪು_PRESPACE;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_ಗುಂಪು(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                 ರಚನೆ fileinfo *infop,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  ಆಯ್ಕೆ(parser->state.UNIX.sub.ಗುಂಪು) {
  ಸ್ಥಿತಿ PL_UNIX_ಗುಂಪು_PRESPACE:
    ಆದರೆ(c != ' ' && len) {
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
      parser->state.UNIX.sub.ಗುಂಪು = PL_UNIX_ಗುಂಪು_NAME;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಗುಂಪು_NAME:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.ಗುಂಪು = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_ಗಾತ್ರ;
      parser->state.UNIX.sub.ಗಾತ್ರ_2 = PL_UNIX_ಗಾತ್ರ_PRESPACE;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_ಗಾತ್ರ(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                ರಚನೆ fileinfo *infop,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  ಆಯ್ಕೆ(parser->state.UNIX.sub.ಗಾತ್ರ_2) {
  ಸ್ಥಿತಿ PL_UNIX_ಗಾತ್ರ_PRESPACE:
    ಆದರೆ(c != ' ') {
      ಆದರೆ(ISDIGIT(c) && len) {
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
        parser->state.UNIX.sub.ಗಾತ್ರ_2 = PL_UNIX_ಗಾತ್ರ_ಸಂಖ್ಯೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಗಾತ್ರ_ಸಂಖ್ಯೆ:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = mem + parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      curl_off_t fsize;
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      ಆದರೆ(!curlx_str_numblanks(&p, &fsize)) {
        ಆದರೆ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_ದತ್ತಾಂಶ->info.flags |= CURLFINFOFLAG_ತಿಳಿದ_ಗಾತ್ರ;
          parser->file_ದತ್ತಾಂಶ->info.ಗಾತ್ರ_2 = fsize;
        }
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
        parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_ಸಮಯ;
        parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PREPART1;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ISDIGIT(c))
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_ಸಮಯ(ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                ರಚನೆ fileinfo *infop,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  ರಚನೆ curl_fileinfo *finfo = &infop->info;

  ಆಯ್ಕೆ(parser->state.UNIX.sub.ಸಮಯ) {
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PREPART1:
    ಆದರೆ(c != ' ') {
      ಆದರೆ(ISALNUM(c) && len) {
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
        parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PART1;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PART1:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ')
      parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PREPART2;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ISALNUM(c) && c != '.')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PREPART2:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c != ' ') {
      ಆದರೆ(ISALNUM(c))
        parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PART2;
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PART2:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ')
      parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PREPART3;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ISALNUM(c) && c != '.')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PREPART3:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c != ' ') {
      ಆದರೆ(ISALNUM(c))
        parser->state.UNIX.sub.ಸಮಯ = PL_UNIX_ಸಮಯ_PART3;
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ_PART3:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.ಸಮಯ = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      ಆದರೆ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ISALNUM(c) && c != '.' && c != ':')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_filename(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                    ರಚನೆ fileinfo *infop,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(parser->state.UNIX.sub.filename) {
  ಸ್ಥಿತಿ PL_UNIX_FILENAME_PRESPACE:
    ಆದರೆ(c != ' ' && len) {
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_FILENAME_NAME:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\n') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.filename = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
      ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_FILENAME_WINDOWSEOL:
    ಆದರೆ(c == '\n') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.filename = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
      ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_FTP_BAD_FILE_LIST;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_unix_symlink(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                                   ರಚನೆ fileinfo *infop,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(parser->state.UNIX.sub.symlink) {
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_PRESPACE:
    ಆದರೆ(c != ' ' && len) {
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_NAME:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\r' || c == '\n')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_PRETARGET1:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\r' || c == '\n')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ಇಲ್ಲದಿದ್ದರೆ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_PRETARGET2:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\r' || c == '\n')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ಇಲ್ಲದಿದ್ದರೆ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_PRETARGET3:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 4] = 0;
      parser->offsets.filename = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\r' || c == '\n')
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ಇಲ್ಲದಿದ್ದರೆ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_PRETARGET4:
    ಆದರೆ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_ಗುರಿ;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_ಗುರಿ:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\n') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.symlink_ಗುರಿ = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;

      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK_WINDOWSEOL:
    ಆದರೆ(c == '\n') {
      mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
      parser->offsets.symlink_ಗುರಿ = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
      ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;

      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_FILETYPE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಫಲಿತಾಂಶ = CURLE_FTP_BAD_FILE_LIST;

    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_unix(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                           ರಚನೆ fileinfo *infop,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  ರಚನೆ curl_fileinfo *finfo = &infop->info;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(parser->state.UNIX.ಮುಖ್ಯ) {
  ಸ್ಥಿತಿ PL_UNIX_TOTALSIZE:
    ಫಲಿತಾಂಶ = parse_unix_totalsize(parser, infop, c);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
    ಆದರೆ(parser->state.UNIX.ಮುಖ್ಯ != PL_UNIX_FILETYPE)
      ನಿಲ್ಲಿಸು;
    FALLTHROUGH();
  ಸ್ಥಿತಿ PL_UNIX_FILETYPE:
    ಫಲಿತಾಂಶ = unix_filetype(c, &finfo->filetype);
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      parser->state.UNIX.ಮುಖ್ಯ = PL_UNIX_PERMISSION;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 1;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_PERMISSION:
    ಫಲಿತಾಂಶ = parse_unix_permission(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_HLINKS:
    ಫಲಿತಾಂಶ = parse_unix_hlinks(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_USER:
    ಫಲಿತಾಂಶ = parse_unix_user(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಗುಂಪು:
    ಫಲಿತಾಂಶ = parse_unix_ಗುಂಪು(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಗಾತ್ರ:
    ಫಲಿತಾಂಶ = parse_unix_ಗಾತ್ರ(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_ಸಮಯ:
    ಫಲಿತಾಂಶ = parse_unix_ಸಮಯ(parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_FILENAME:
    ಫಲಿತಾಂಶ = parse_unix_filename(ದತ್ತಾಂಶ, parser, infop, c);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_UNIX_SYMLINK:
    ಫಲಿತಾಂಶ = parse_unix_symlink(ದತ್ತಾಂಶ, parser, infop, c);
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode parse_winnt(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser,
                            ರಚನೆ fileinfo *infop,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c)
{
  ರಚನೆ curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  ಅಕ್ಷರ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆಯ್ಕೆ(parser->state.NT.ಮುಖ್ಯ) {
  ಸ್ಥಿತಿ PL_WINNT_DATE:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆದರೆ(parser->ಅಂಶ_ಉದ್ದಳತೆ < 9) {
      ಆದರೆ(!strchr("0123456789-", c)) { /* only simple control */
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(parser->ಅಂಶ_ಉದ್ದಳತೆ == 9) {
      ಆದರೆ(c == ' ') {
        parser->state.NT.ಮುಖ್ಯ = PL_WINNT_ಸಮಯ;
        parser->state.NT.sub.ಸಮಯ = PL_WINNT_ಸಮಯ_PRESPACE;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_WINNT_ಸಮಯ:
    parser->ಅಂಶ_ಉದ್ದಳತೆ++;
    ಆಯ್ಕೆ(parser->state.NT.sub.ಸಮಯ) {
    ಸ್ಥಿತಿ PL_WINNT_ಸಮಯ_PRESPACE:
      ಆದರೆ(!ISBLANK(c))
        parser->state.NT.sub.ಸಮಯ = PL_WINNT_ಸಮಯ_ಸಮಯ;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ PL_WINNT_ಸಮಯ_ಸಮಯ:
      ಆದರೆ(c == ' ') {
        parser->offsets.ಸಮಯ = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
        mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
        parser->state.NT.ಮುಖ್ಯ = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strchr("APM0123456789:", c))
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_WINNT_DIRORSIZE:
    ಆಯ್ಕೆ(parser->state.NT.sub.dirorsize) {
    ಸ್ಥಿತಿ PL_WINNT_DIRORSIZE_PRESPACE:
      ಆದರೆ(c != ' ' && len) {
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ PL_WINNT_DIRORSIZE_CONTENT:
      parser->ಅಂಶ_ಉದ್ದಳತೆ++;
      ಆದರೆ(c == ' ') {
        mem[parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ + parser->ಅಂಶ_ಉದ್ದಳತೆ - 1] = 0;
        ಆದರೆ(!strcmp("<DIR>", mem + parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->ಗಾತ್ರ_2 = 0;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = mem + parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
          ಆದರೆ(curlx_str_numblanks(&p, &finfo->ಗಾತ್ರ_2)) {
            ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_ದತ್ತಾಂಶ->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_ದತ್ತಾಂಶ->info.flags |= CURLFINFOFLAG_ತಿಳಿದ_ಗಾತ್ರ;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
        parser->state.NT.ಮುಖ್ಯ = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ PL_WINNT_FILENAME:
    ಆಯ್ಕೆ(parser->state.NT.sub.filename) {
    ಸ್ಥಿತಿ PL_WINNT_FILENAME_PRESPACE:
      ಆದರೆ(c != ' ' && len) {
        parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = len - 1;
        parser->ಅಂಶ_ಉದ್ದಳತೆ = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ PL_WINNT_FILENAME_CONTENT:
      parser->ಅಂಶ_ಉದ್ದಳತೆ++;
      ಆದರೆ(!len)
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;
      ಆದರೆ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\n') {
        parser->offsets.filename = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
        mem[len - 1] = 0;
        ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        parser->state.NT.ಮುಖ್ಯ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ PL_WINNT_FILENAME_WINEOL:
      ಆದರೆ(c == '\n') {
        parser->offsets.filename = parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ;
        ಫಲಿತಾಂಶ = ftp_pl_ಸೇರಿಸು_finfo(ದತ್ತಾಂಶ, infop);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

        parser->state.NT.ಮುಖ್ಯ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_FTP_BAD_FILE_LIST;

      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(ದತ್ತಾಂಶ->state.wildcardmatch) {
    ರಚನೆ Wildcard_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ wildcard = ದತ್ತಾಂಶ->wildcard;
    ಹಿಂತಿರುಗಿ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

size_t Curl_ftp_parselist(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2, size_t nmemb,
                          ಶೂನ್ಯ *connptr)
{
  size_t bufflen = ಗಾತ್ರ_2 * nmemb;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)connptr;
  ರಚನೆ ftp_wc *ftpwc = ದತ್ತಾಂಶ->wildcard->ftpwc;
  ರಚನೆ ftp_parselist_ದತ್ತಾಂಶ *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode ಫಲಿತಾಂಶ;
  size_t retsize = bufflen;

  ಆದರೆ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    ಹೋಗು fail;
  }

  ಆದರೆ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(ಬಫರ್[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ಆಗ(i < bufflen) { /* FSM */
    ಅಕ್ಷರ c = ಬಫರ್[i];
    ರಚನೆ fileinfo *infop;
    ಆದರೆ(!parser->file_ದತ್ತಾಂಶ) { /* tmp file data is not allocated yet */
      parser->file_ದತ್ತಾಂಶ = Curl_fileinfo_alloc();
      ಆದರೆ(!parser->file_ದತ್ತಾಂಶ) {
        parser->error = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು fail;
      }
      parser->ಅಂಶ_ಸ್ಥಾನಾಂತರ = 0;
      parser->ಅಂಶ_ಉದ್ದಳತೆ = 0;
      curlx_dyn_init(&parser->file_ದತ್ತಾಂಶ->buf, MAX_FTPLIST_ಬಫರ್);
    }

    infop = parser->file_ದತ್ತಾಂಶ;

    ಆದರೆ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು fail;
    }

    ಆಯ್ಕೆ(parser->os_type) {
    ಸ್ಥಿತಿ OS_TYPE_UNIX:
      ಫಲಿತಾಂಶ = parse_unix(ದತ್ತಾಂಶ, parser, infop, c);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ OS_TYPE_WIN_NT:
      ಫಲಿತಾಂಶ = parse_winnt(ದತ್ತಾಂಶ, parser, infop, c);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      retsize = bufflen + 1;
      ಹೋಗು fail;
    }
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      parser->error = ಫಲಿತಾಂಶ;
      ಹೋಗು fail;
    }

    i++;
  }
  ಹಿಂತಿರುಗಿ retsize;

fail:

  /* Clean up any allocated memory. */
  ಆದರೆ(parser->file_ದತ್ತಾಂಶ) {
    Curl_fileinfo_cleanup(parser->file_ದತ್ತಾಂಶ);
    parser->file_ದತ್ತಾಂಶ = ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ retsize;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_FTP */
