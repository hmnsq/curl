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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "fileinfo.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "ftp-int.h"
#ಅನ್ತರ್ಭಾವ "ftplistparser.h"
#ಅನ್ತರ್ಭಾವ "curl_fnmatch.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  PL_UNIX_TOTALSIZE = 0,
  PL_UNIX_FILETYPE,
  PL_UNIX_PERMISSION,
  PL_UNIX_HLINKS,
  PL_UNIX_USER,
  PL_UNIX_GROUP,
  PL_UNIX_SIZE,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ {
  ಗಣನಾ {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  ಗಣನಾ {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_ಸಂಖ್ಯೆ
  } hlinks;

  ಗಣನಾ {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ಗಣನಾ {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  ಗಣನಾ {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_ಸಂಖ್ಯೆ
  } size;

  ಗಣನಾ {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  ಗಣನಾ {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ಗಣನಾ {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ {
  ಗಣನಾ {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  ಗಣನಾ {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ಗಣನಾ {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ಸಂರಚನಾ ftp_parselist_data {
  ಗಣನಾ {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  ಸಙ್ಘ {
    ಸಂರಚನಾ {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    ಸಂರಚನಾ {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ಸಂರಚನಾ fileinfo *file_data;
  unsigned ಪೂರ್ಣಾಙ್ಕ item_length;
  size_t item_offset;
  ಸಂರಚನಾ {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

ಸ್ಥಿರ ಶೂನ್ಯ fileinfo_dtor(ಶೂನ್ಯ *user, ಶೂನ್ಯ *element)
{
  (ಶೂನ್ಯ)user;
  Curl_fileinfo_cleanup(element);
}

ಶೂನ್ಯ Curl_wildcard_init(ಸಂರಚನಾ WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

ಶೂನ್ಯ Curl_wildcard_dtor(ಸಂರಚನಾ WildcardData **wcp)
{
  ಸಂರಚನಾ WildcardData *wc = *wcp;
  ಯದಿ(!wc)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = ರಿಕ್ತ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, ರಿಕ್ತ);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = ರಿಕ್ತ;
}

ಸಂರಚನಾ ftp_parselist_data *Curl_ftp_parselist_data_alloc(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ ftp_parselist_data));
}

ಶೂನ್ಯ Curl_ftp_parselist_data_free(ಸಂರಚನಾ ftp_parselist_data **parserp)
{
  ಸಂರಚನಾ ftp_parselist_data *parser = *parserp;
  ಯದಿ(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = ರಿಕ್ತ;
}

CURLcode Curl_ftp_parselist_geterror(ಸಂರಚನಾ ftp_parselist_data *pl_data)
{
  ಪ್ರತ್ಯಾಗಮ pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ ftp_pl_get_permission(ಧ್ರುವ ವರ್ಣ *str)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ permissions = 0;
  /* USER */
  ಯದಿ(str[0] == 'r')
    permissions |= 1 << 8;
  ಅನ್ಯಥಾ ಯದಿ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಯದಿ(str[1] == 'w')
    permissions |= 1 << 7;
  ಅನ್ಯಥಾ ಯದಿ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ಯದಿ(str[2] == 'x')
    permissions |= 1 << 6;
  ಅನ್ಯಥಾ ಯದಿ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ಅನ್ಯಥಾ ಯದಿ(str[2] == 'S')
    permissions |= 1 << 11;
  ಅನ್ಯಥಾ ಯದಿ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  ಯದಿ(str[3] == 'r')
    permissions |= 1 << 5;
  ಅನ್ಯಥಾ ಯದಿ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಯದಿ(str[4] == 'w')
    permissions |= 1 << 4;
  ಅನ್ಯಥಾ ಯದಿ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಯದಿ(str[5] == 'x')
    permissions |= 1 << 3;
  ಅನ್ಯಥಾ ಯದಿ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ಅನ್ಯಥಾ ಯದಿ(str[5] == 'S')
    permissions |= 1 << 10;
  ಅನ್ಯಥಾ ಯದಿ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ಯದಿ(str[6] == 'r')
    permissions |= 1 << 2;
  ಅನ್ಯಥಾ ಯದಿ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಯದಿ(str[7] == 'w')
    permissions |= 1 << 1;
  ಅನ್ಯಥಾ ಯದಿ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ಯದಿ(str[8] == 'x')
    permissions |= 1;
  ಅನ್ಯಥಾ ಯದಿ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ಅನ್ಯಥಾ ಯದಿ(str[8] == 'T')
    permissions |= 1 << 9;
  ಅನ್ಯಥಾ ಯದಿ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ಪ್ರತ್ಯಾಗಮ permissions;
}

ಸ್ಥಿರ CURLcode ftp_pl_ಸೇರಿಸು_finfo(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ಸಂರಚನಾ WildcardData *wc = data->wildcard;
  ಸಂರಚನಾ ftp_wc *ftpwc = wc->ftpwc;
  ಸಂರಚನಾ Curl_llist *llist = &wc->filelist;
  ಸಂರಚನಾ ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  ಸಂರಚನಾ curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  ವರ್ಣ *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : ರಿಕ್ತ;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : ರಿಕ್ತ;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : ರಿಕ್ತ;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : ರಿಕ್ತ;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  ಯದಿ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ಯದಿ(strchr(finfo->filename, '/'))
    add = FALSE;
  ಅನ್ಯಥಾ {
    /* a real file name cannot contain a slash */
    ಸಂರಚನಾ Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    ಯದಿ(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ಯದಿ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    ಅನ್ಯಥಾ {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  ಯದಿ(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ಅನ್ಯಥಾ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

ಸ್ಥಿರ CURLcode unix_filetype(ಧ್ರುವ ವರ್ಣ c, curlfiletype *t)
{
  ವಿಕಲ್ಪ(c) {
  ಪಕ್ಷ '-':
    *t = CURLFILETYPE_FILE;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'd':
    *t = CURLFILETYPE_DIRECTORY;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'l':
    *t = CURLFILETYPE_SYMLINK;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    ಭಙ್ಗ;
  ಪಕ್ಷ 's':
    *t = CURLFILETYPE_SOCKET;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'D':
    *t = CURLFILETYPE_DOOR;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_totalsize(ಸಂರಚನಾ ftp_parselist_data *parser,
                                     ಸಂರಚನಾ fileinfo *infop,
                                     ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  ವಿಕಲ್ಪ(parser->state.UNIX.sub.total_dirsize) {
  ಪಕ್ಷ PL_UNIX_TOTALSIZE_INIT:
    ಯದಿ(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    ಅನ್ಯಥಾ {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    ಯದಿ(c == '\r') {
      parser->item_length--;
      ಯದಿ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ಅನ್ಯಥಾ ಯದಿ(c == '\n') {
      mem[parser->item_length - 1] = 0;
      ಯದಿ(!strncmp("total ", mem, 6)) {
        ಧ್ರುವ ವರ್ಣ *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ಯಾವತ್(ISDIGIT(*endptr))
          endptr++;
        ಯದಿ(*endptr) {
          ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_permission(ಸಂರಚನಾ ftp_parselist_data *parser,
                                      ಸಂರಚನಾ fileinfo *infop,
                                      ಧ್ರುವ ವರ್ಣ c)
{
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  ಯದಿ((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

  ಅನ್ಯಥಾ ಯದಿ(parser->item_length == 10) {
    unsigned ಪೂರ್ಣಾಙ್ಕ perm;
    ಯದಿ(c != ' ')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    ಯದಿ(perm & FTP_LP_MALFORMATED_PERM)
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_hlinks(ಸಂರಚನಾ ftp_parselist_data *parser,
                                  ಸಂರಚನಾ fileinfo *infop,
                                  ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);

  ವಿಕಲ್ಪ(parser->state.UNIX.sub.hlinks) {
  ಪಕ್ಷ PL_UNIX_HLINKS_PRESPACE:
    ಯದಿ(c != ' ') {
      ಯದಿ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_ಸಂಖ್ಯೆ;
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_HLINKS_ಸಂಖ್ಯೆ:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      ಧ್ರುವ ವರ್ಣ *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ಅನ್ಯಥಾ ಯದಿ(!ISDIGIT(c))
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_user(ಸಂರಚನಾ ftp_parselist_data *parser,
                                ಸಂರಚನಾ fileinfo *infop,
                                ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  ವಿಕಲ್ಪ(parser->state.UNIX.sub.user) {
  ಪಕ್ಷ PL_UNIX_USER_PRESPACE:
    ಯದಿ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_USER_PARSING:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_group(ಸಂರಚನಾ ftp_parselist_data *parser,
                                 ಸಂರಚನಾ fileinfo *infop,
                                 ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  ವಿಕಲ್ಪ(parser->state.UNIX.sub.group) {
  ಪಕ್ಷ PL_UNIX_GROUP_PRESPACE:
    ಯದಿ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_GROUP_NAME:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_size(ಸಂರಚನಾ ftp_parselist_data *parser,
                                ಸಂರಚನಾ fileinfo *infop,
                                ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  ವಿಕಲ್ಪ(parser->state.UNIX.sub.size) {
  ಪಕ್ಷ PL_UNIX_SIZE_PRESPACE:
    ಯದಿ(c != ' ') {
      ಯದಿ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_ಸಂಖ್ಯೆ;
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SIZE_ಸಂಖ್ಯೆ:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      ಧ್ರುವ ವರ್ಣ *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      ಯದಿ(!curlx_str_numblanks(&p, &fsize)) {
        ಯದಿ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(!ISDIGIT(c))
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_time(ಸಂರಚನಾ ftp_parselist_data *parser,
                                ಸಂರಚನಾ fileinfo *infop,
                                ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  ಸಂರಚನಾ curl_fileinfo *finfo = &infop->info;

  ವಿಕಲ್ಪ(parser->state.UNIX.sub.time) {
  ಪಕ್ಷ PL_UNIX_TIME_PREPART1:
    ಯದಿ(c != ' ') {
      ಯದಿ(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME_PART1:
    parser->item_length++;
    ಯದಿ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    ಅನ್ಯಥಾ ಯದಿ(!ISALNUM(c) && c != '.')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    ಯದಿ(c != ' ') {
      ಯದಿ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME_PART2:
    parser->item_length++;
    ಯದಿ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    ಅನ್ಯಥಾ ಯದಿ(!ISALNUM(c) && c != '.')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    ಯದಿ(c != ' ') {
      ಯದಿ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME_PART3:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      ಯದಿ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ಅನ್ಯಥಾ {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(!ISALNUM(c) && c != '.' && c != ':')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode parse_unix_filename(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ ftp_parselist_data *parser,
                                    ಸಂರಚನಾ fileinfo *infop,
                                    ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(parser->state.UNIX.sub.filename) {
  ಪಕ್ಷ PL_UNIX_FILENAME_PRESPACE:
    ಯದಿ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    ಯದಿ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ಅನ್ಯಥಾ ಯದಿ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_FILENAME_WINDOWSEOL:
    ಯದಿ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
    }
    ಅನ್ಯಥಾ
      result = CURLE_FTP_BAD_FILE_LIST;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_unix_symlink(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ftp_parselist_data *parser,
                                   ಸಂರಚನಾ fileinfo *infop,
                                   ಧ್ರುವ ವರ್ಣ c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(parser->state.UNIX.sub.symlink) {
  ಪಕ್ಷ PL_UNIX_SYMLINK_PRESPACE:
    ಯದಿ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    ಯದಿ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ಅನ್ಯಥಾ ಯದಿ(c == '\r' || c == '\n')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    ಯದಿ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ಅನ್ಯಥಾ ಯದಿ(c == '\r' || c == '\n')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಅನ್ಯಥಾ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    ಯದಿ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ಅನ್ಯಥಾ ಯದಿ(c == '\r' || c == '\n')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಅನ್ಯಥಾ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    ಯದಿ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ(c == '\r' || c == '\n')
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಅನ್ಯಥಾ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_PRETARGET4:
    ಯದಿ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    ಯದಿ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ಅನ್ಯಥಾ ಯದಿ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
      ಯದಿ(result)
        ಭಙ್ಗ;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK_WINDOWSEOL:
    ಯದಿ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
      ಯದಿ(result)
        ಭಙ್ಗ;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    ಅನ್ಯಥಾ
      result = CURLE_FTP_BAD_FILE_LIST;

    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_unix(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ ftp_parselist_data *parser,
                           ಸಂರಚನಾ fileinfo *infop,
                           ಧ್ರುವ ವರ್ಣ c)
{
  ಸಂರಚನಾ curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(parser->state.UNIX.main) {
  ಪಕ್ಷ PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    ಯದಿ(result)
      ಭಙ್ಗ;
    ಯದಿ(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      ಭಙ್ಗ;
    FALLTHROUGH();
  ಪಕ್ಷ PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    ಯದಿ(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_winnt(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ftp_parselist_data *parser,
                            ಸಂರಚನಾ fileinfo *infop,
                            ಧ್ರುವ ವರ್ಣ c)
{
  ಸಂರಚನಾ curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  ವರ್ಣ *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  ವಿಕಲ್ಪ(parser->state.NT.main) {
  ಪಕ್ಷ PL_WINNT_DATE:
    parser->item_length++;
    ಯದಿ(parser->item_length < 9) {
      ಯದಿ(!strchr("0123456789-", c)) { /* only simple control */
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(parser->item_length == 9) {
      ಯದಿ(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_WINNT_TIME:
    parser->item_length++;
    ವಿಕಲ್ಪ(parser->state.NT.sub.time) {
    ಪಕ್ಷ PL_WINNT_TIME_PRESPACE:
      ಯದಿ(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      ಭಙ್ಗ;
    ಪಕ್ಷ PL_WINNT_TIME_TIME:
      ಯದಿ(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      ಅನ್ಯಥಾ ಯದಿ(!strchr("APM0123456789:", c))
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_WINNT_DIRORSIZE:
    ವಿಕಲ್ಪ(parser->state.NT.sub.dirorsize) {
    ಪಕ್ಷ PL_WINNT_DIRORSIZE_PRESPACE:
      ಯದಿ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      ಯದಿ(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        ಯದಿ(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        ಅನ್ಯಥಾ {
          ಧ್ರುವ ವರ್ಣ *p = mem + parser->item_offset;
          ಯದಿ(curlx_str_numblanks(&p, &finfo->size)) {
            ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ PL_WINNT_FILENAME:
    ವಿಕಲ್ಪ(parser->state.NT.sub.filename) {
    ಪಕ್ಷ PL_WINNT_FILENAME_PRESPACE:
      ಯದಿ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      ಯದಿ(!len)
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;
      ಯದಿ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ PL_WINNT_FILENAME_WINEOL:
      ಯದಿ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_ಸೇರಿಸು_finfo(data, infop);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_FTP_BAD_FILE_LIST;

      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(data->state.wildcardmatch) {
    ಸಂರಚನಾ WildcardData * ಧ್ರುವ wildcard = data->wildcard;
    ಪ್ರತ್ಯಾಗಮ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

size_t Curl_ftp_parselist(ವರ್ಣ *buffer, size_t size, size_t nmemb,
                          ಶೂನ್ಯ *connptr)
{
  size_t bufflen = size * nmemb;
  ಸಂರಚನಾ Curl_easy *data = (ಸಂರಚನಾ Curl_easy *)connptr;
  ಸಂರಚನಾ ftp_wc *ftpwc = data->wildcard->ftpwc;
  ಸಂರಚನಾ ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  ಯದಿ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    ಗಚ್ಛ fail;
  }

  ಯದಿ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ಯಾವತ್(i < bufflen) { /* FSM */
    ವರ್ಣ c = buffer[i];
    ಸಂರಚನಾ fileinfo *infop;
    ಯದಿ(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      ಯದಿ(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    ಯದಿ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ fail;
    }

    ವಿಕಲ್ಪ(parser->os_type) {
    ಪಕ್ಷ OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      ಭಙ್ಗ;
    ಪಕ್ಷ OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      retsize = bufflen + 1;
      ಗಚ್ಛ fail;
    }
    ಯದಿ(result) {
      parser->error = result;
      ಗಚ್ಛ fail;
    }

    i++;
  }
  ಪ್ರತ್ಯಾಗಮ retsize;

fail:

  /* Clean up any allocated memory. */
  ಯದಿ(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ retsize;
}

#endif /* !CURL_DISABLE_FTP */
