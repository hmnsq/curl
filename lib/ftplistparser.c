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

#شامل "urldata.h"
#شامل "fileinfo.h"
#شامل "llist.h"
#شامل "ftp.h"
#شامل "ftp-int.h"
#شامل "ftplistparser.h"
#شامل "curl_fnmatch.h"
#شامل "multiif.h"
#شامل "curlx/strparse.h"

تعریفنوع شمارش {
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

تعریفنوع اجتماع {
  شمارش {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  شمارش {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_عدد
  } hlinks;

  شمارش {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  شمارش {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  شمارش {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_عدد
  } size;

  شمارش {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  شمارش {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  شمارش {
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

تعریفنوع شمارش {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

تعریفنوع اجتماع {
  شمارش {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  شمارش {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  شمارش {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
ساختار ftp_parselist_data {
  شمارش {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  اجتماع {
    ساختار {
      pl_unix_mainstate اصلی;
      pl_unix_substate sub;
    } UNIX;

    ساختار {
      pl_winNT_mainstate اصلی;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  ساختار fileinfo *file_data;
  unsigned صحیح item_length;
  size_t item_offset;
  ساختار {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

ایستا تهی fileinfo_dtor(تهی *user, تهی *element)
{
  (تهی)user;
  Curl_fileinfo_cleanup(element);
}

تهی Curl_wildcard_init(ساختار WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

تهی Curl_wildcard_dtor(ساختار WildcardData **wcp)
{
  ساختار WildcardData *wc = *wcp;
  اگر(!wc)
    بازگشت;

  اگر(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = پوچ;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, پوچ);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = پوچ;
}

ساختار ftp_parselist_data *Curl_ftp_parselist_data_alloc(تهی)
{
  بازگشت curlx_calloc(1, اندازه(ساختار ftp_parselist_data));
}

تهی Curl_ftp_parselist_data_free(ساختار ftp_parselist_data **parserp)
{
  ساختار ftp_parselist_data *parser = *parserp;
  اگر(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = پوچ;
}

CURLcode Curl_ftp_parselist_geterror(ساختار ftp_parselist_data *pl_data)
{
  بازگشت pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

ایستا unsigned صحیح ftp_pl_get_permission(ثابت نویسه *str)
{
  unsigned صحیح permissions = 0;
  /* USER */
  اگر(str[0] == 'r')
    permissions |= 1 << 8;
  وگرنه اگر(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[1] == 'w')
    permissions |= 1 << 7;
  وگرنه اگر(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  اگر(str[2] == 'x')
    permissions |= 1 << 6;
  وگرنه اگر(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  وگرنه اگر(str[2] == 'S')
    permissions |= 1 << 11;
  وگرنه اگر(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  اگر(str[3] == 'r')
    permissions |= 1 << 5;
  وگرنه اگر(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[4] == 'w')
    permissions |= 1 << 4;
  وگرنه اگر(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[5] == 'x')
    permissions |= 1 << 3;
  وگرنه اگر(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  وگرنه اگر(str[5] == 'S')
    permissions |= 1 << 10;
  وگرنه اگر(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  اگر(str[6] == 'r')
    permissions |= 1 << 2;
  وگرنه اگر(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[7] == 'w')
    permissions |= 1 << 1;
  وگرنه اگر(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  اگر(str[8] == 'x')
    permissions |= 1;
  وگرنه اگر(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  وگرنه اگر(str[8] == 'T')
    permissions |= 1 << 9;
  وگرنه اگر(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  بازگشت permissions;
}

ایستا CURLcode ftp_pl_درج_finfo(ساختار Curl_easy *data,
                                    ساختار fileinfo *infop)
{
  curl_fnmatch_callback compare;
  ساختار WildcardData *wc = data->wildcard;
  ساختار ftp_wc *ftpwc = wc->ftpwc;
  ساختار Curl_llist *llist = &wc->filelist;
  ساختار ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  ساختار curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  نویسه *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : پوچ;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : پوچ;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : پوچ;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : پوچ;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  اگر(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  اگر(strchr(finfo->filename, '/'))
    add = FALSE;
  وگرنه {
    /* a real file name cannot contain a slash */
    ساختار Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    اگر(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      اگر((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    وگرنه {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  اگر(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  وگرنه {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = پوچ;
  بازگشت CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

ایستا CURLcode unix_filetype(ثابت نویسه c, curlfiletype *t)
{
  انتخاب(c) {
  حالت '-':
    *t = CURLFILETYPE_FILE;
    بشکن;
  حالت 'd':
    *t = CURLFILETYPE_DIRECTORY;
    بشکن;
  حالت 'l':
    *t = CURLFILETYPE_SYMLINK;
    بشکن;
  حالت 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    بشکن;
  حالت 's':
    *t = CURLFILETYPE_SOCKET;
    بشکن;
  حالت 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    بشکن;
  حالت 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    بشکن;
  حالت 'D':
    *t = CURLFILETYPE_DOOR;
    بشکن;
  پیشفرض:
    بازگشت CURLE_FTP_BAD_FILE_LIST;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_totalsize(ساختار ftp_parselist_data *parser,
                                     ساختار fileinfo *infop,
                                     ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.total_dirsize) {
  حالت PL_UNIX_TOTALSIZE_INIT:
    اگر(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    وگرنه {
      parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    بشکن;
  حالت PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    اگر(c == '\r') {
      parser->item_length--;
      اگر(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    وگرنه اگر(c == '\n') {
      mem[parser->item_length - 1] = 0;
      اگر(!strncmp("total ", mem, 6)) {
        ثابت نویسه *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        وقتی(ISDIGIT(*endptr))
          endptr++;
        اگر(*endptr) {
          بازگشت CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_permission(ساختار ftp_parselist_data *parser,
                                      ساختار fileinfo *infop,
                                      ثابت نویسه c)
{
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  اگر((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    بازگشت CURLE_FTP_BAD_FILE_LIST;

  وگرنه اگر(parser->item_length == 10) {
    unsigned صحیح perm;
    اگر(c != ' ')
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    اگر(perm & FTP_LP_MALFORMATED_PERM)
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.اصلی = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_hlinks(ساختار ftp_parselist_data *parser,
                                  ساختار fileinfo *infop,
                                  ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);

  انتخاب(parser->state.UNIX.sub.hlinks) {
  حالت PL_UNIX_HLINKS_PRESPACE:
    اگر(c != ' ') {
      اگر(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_عدد;
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  حالت PL_UNIX_HLINKS_عدد:
    parser->item_length++;
    اگر(c == ' ') {
      ثابت نویسه *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      اگر(!curlx_str_عدد(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.اصلی = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    وگرنه اگر(!ISDIGIT(c))
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_user(ساختار ftp_parselist_data *parser,
                                ساختار fileinfo *infop,
                                ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.user) {
  حالت PL_UNIX_USER_PRESPACE:
    اگر(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    بشکن;
  حالت PL_UNIX_USER_PARSING:
    parser->item_length++;
    اگر(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.اصلی = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_group(ساختار ftp_parselist_data *parser,
                                 ساختار fileinfo *infop,
                                 ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.group) {
  حالت PL_UNIX_GROUP_PRESPACE:
    اگر(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    بشکن;
  حالت PL_UNIX_GROUP_NAME:
    parser->item_length++;
    اگر(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.اصلی = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_size(ساختار ftp_parselist_data *parser,
                                ساختار fileinfo *infop,
                                ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  انتخاب(parser->state.UNIX.sub.size) {
  حالت PL_UNIX_SIZE_PRESPACE:
    اگر(c != ' ') {
      اگر(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_عدد;
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  حالت PL_UNIX_SIZE_عدد:
    parser->item_length++;
    اگر(c == ' ') {
      ثابت نویسه *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      اگر(!curlx_str_numblanks(&p, &fsize)) {
        اگر(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.اصلی = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    وگرنه اگر(!ISDIGIT(c))
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_time(ساختار ftp_parselist_data *parser,
                                ساختار fileinfo *infop,
                                ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  ساختار curl_fileinfo *finfo = &infop->info;

  انتخاب(parser->state.UNIX.sub.time) {
  حالت PL_UNIX_TIME_PREPART1:
    اگر(c != ' ') {
      اگر(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  حالت PL_UNIX_TIME_PART1:
    parser->item_length++;
    اگر(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    وگرنه اگر(!ISALNUM(c) && c != '.')
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  حالت PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    اگر(c != ' ') {
      اگر(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  حالت PL_UNIX_TIME_PART2:
    parser->item_length++;
    اگر(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    وگرنه اگر(!ISALNUM(c) && c != '.')
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    بشکن;
  حالت PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    اگر(c != ' ') {
      اگر(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    بشکن;
  حالت PL_UNIX_TIME_PART3:
    parser->item_length++;
    اگر(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      اگر(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.اصلی = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      وگرنه {
        parser->state.UNIX.اصلی = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    وگرنه اگر(!ISALNUM(c) && c != '.' && c != ':')
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode parse_unix_filename(ساختار Curl_easy *data,
                                    ساختار ftp_parselist_data *parser,
                                    ساختار fileinfo *infop,
                                    ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  انتخاب(parser->state.UNIX.sub.filename) {
  حالت PL_UNIX_FILENAME_PRESPACE:
    اگر(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    بشکن;
  حالت PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    اگر(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    وگرنه اگر(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
      result = ftp_pl_درج_finfo(data, infop);
    }
    بشکن;
  حالت PL_UNIX_FILENAME_WINDOWSEOL:
    اگر(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
      result = ftp_pl_درج_finfo(data, infop);
    }
    وگرنه
      result = CURLE_FTP_BAD_FILE_LIST;
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode parse_unix_symlink(ساختار Curl_easy *data,
                                   ساختار ftp_parselist_data *parser,
                                   ساختار fileinfo *infop,
                                   ثابت نویسه c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  انتخاب(parser->state.UNIX.sub.symlink) {
  حالت PL_UNIX_SYMLINK_PRESPACE:
    اگر(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    بشکن;
  حالت PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    اگر(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    وگرنه اگر(c == '\r' || c == '\n')
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  حالت PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    اگر(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    وگرنه اگر(c == '\r' || c == '\n')
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    وگرنه
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    بشکن;
  حالت PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    اگر(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    وگرنه اگر(c == '\r' || c == '\n')
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    وگرنه
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    بشکن;
  حالت PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    اگر(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    وگرنه اگر(c == '\r' || c == '\n')
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    وگرنه
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    بشکن;
  حالت PL_UNIX_SYMLINK_PRETARGET4:
    اگر(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    وگرنه
      بازگشت CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  حالت PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    اگر(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    وگرنه اگر(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_درج_finfo(data, infop);
      اگر(result)
        بشکن;

      parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
    }
    بشکن;
  حالت PL_UNIX_SYMLINK_WINDOWSEOL:
    اگر(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_درج_finfo(data, infop);
      اگر(result)
        بشکن;

      parser->state.UNIX.اصلی = PL_UNIX_FILETYPE;
    }
    وگرنه
      result = CURLE_FTP_BAD_FILE_LIST;

    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode parse_unix(ساختار Curl_easy *data,
                           ساختار ftp_parselist_data *parser,
                           ساختار fileinfo *infop,
                           ثابت نویسه c)
{
  ساختار curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  انتخاب(parser->state.UNIX.اصلی) {
  حالت PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    اگر(result)
      بشکن;
    اگر(parser->state.UNIX.اصلی != PL_UNIX_FILETYPE)
      بشکن;
    FALLTHROUGH();
  حالت PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    اگر(!result) {
      parser->state.UNIX.اصلی = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    بشکن;
  حالت PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    بشکن;
  حالت PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    بشکن;
  حالت PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    بشکن;
  حالت PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    بشکن;
  حالت PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    بشکن;
  حالت PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    بشکن;
  حالت PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    بشکن;
  حالت PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    بشکن;
  }
  بازگشت result;
}

ایستا CURLcode parse_winnt(ساختار Curl_easy *data,
                            ساختار ftp_parselist_data *parser,
                            ساختار fileinfo *infop,
                            ثابت نویسه c)
{
  ساختار curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  نویسه *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  انتخاب(parser->state.NT.اصلی) {
  حالت PL_WINNT_DATE:
    parser->item_length++;
    اگر(parser->item_length < 9) {
      اگر(!strchr("0123456789-", c)) { /* only simple control */
        بازگشت CURLE_FTP_BAD_FILE_LIST;
      }
    }
    وگرنه اگر(parser->item_length == 9) {
      اگر(c == ' ') {
        parser->state.NT.اصلی = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;
    }
    وگرنه
      بازگشت CURLE_FTP_BAD_FILE_LIST;
    بشکن;
  حالت PL_WINNT_TIME:
    parser->item_length++;
    انتخاب(parser->state.NT.sub.time) {
    حالت PL_WINNT_TIME_PRESPACE:
      اگر(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      بشکن;
    حالت PL_WINNT_TIME_TIME:
      اگر(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.اصلی = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      وگرنه اگر(!strchr("APM0123456789:", c))
        بازگشت CURLE_FTP_BAD_FILE_LIST;
      بشکن;
    }
    بشکن;
  حالت PL_WINNT_DIRORSIZE:
    انتخاب(parser->state.NT.sub.dirorsize) {
    حالت PL_WINNT_DIRORSIZE_PRESPACE:
      اگر(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      بشکن;
    حالت PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      اگر(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        اگر(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        وگرنه {
          ثابت نویسه *p = mem + parser->item_offset;
          اگر(curlx_str_numblanks(&p, &finfo->size)) {
            بازگشت CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.اصلی = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      بشکن;
    }
    بشکن;
  حالت PL_WINNT_FILENAME:
    انتخاب(parser->state.NT.sub.filename) {
    حالت PL_WINNT_FILENAME_PRESPACE:
      اگر(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      بشکن;
    حالت PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      اگر(!len)
        بازگشت CURLE_FTP_BAD_FILE_LIST;
      اگر(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      وگرنه اگر(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_درج_finfo(data, infop);
        اگر(result)
          بازگشت result;

        parser->state.NT.اصلی = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      بشکن;
    حالت PL_WINNT_FILENAME_WINEOL:
      اگر(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_درج_finfo(data, infop);
        اگر(result)
          بازگشت result;

        parser->state.NT.اصلی = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      وگرنه
        بازگشت CURLE_FTP_BAD_FILE_LIST;

      بشکن;
    }
    بشکن;
  }

  بازگشت CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(ساختار Curl_easy *data)
{
  اگر(data->state.wildcardmatch) {
    ساختار WildcardData * ثابت wildcard = data->wildcard;
    بازگشت wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  بازگشت FALSE;
}

size_t Curl_ftp_parselist(نویسه *buffer, size_t size, size_t nmemb,
                          تهی *connptr)
{
  size_t bufflen = size * nmemb;
  ساختار Curl_easy *data = (ساختار Curl_easy *)connptr;
  ساختار ftp_wc *ftpwc = data->wildcard->ftpwc;
  ساختار ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  اگر(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    برو fail;
  }

  اگر(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  وقتی(i < bufflen) { /* FSM */
    نویسه c = buffer[i];
    ساختار fileinfo *infop;
    اگر(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      اگر(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        برو fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    اگر(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      برو fail;
    }

    انتخاب(parser->os_type) {
    حالت OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      بشکن;
    حالت OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      بشکن;
    پیشفرض:
      retsize = bufflen + 1;
      برو fail;
    }
    اگر(result) {
      parser->error = result;
      برو fail;
    }

    i++;
  }
  بازگشت retsize;

fail:

  /* Clean up any allocated memory. */
  اگر(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = پوچ;
  }

  بازگشت retsize;
}

#endif /* !CURL_DISABLE_FTP */
