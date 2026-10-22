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
  PL_UNIX_GROUP,
  PL_UNIX_SIZE,
  PL_UNIX_TIME,
  PL_UNIX_FILENAME,
  PL_UNIX_SYMLINK
} pl_unix_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  تعداد {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_NUMBER
  } hlinks;

  تعداد {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  تعداد {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  تعداد {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_NUMBER
  } size;

  تعداد {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

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
    PL_UNIX_SYMLINK_TARGET,
    PL_UNIX_SYMLINK_WINDOWSEOL
  } symlink;
} pl_unix_substate;

تعريفنوع تعداد {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

تعريفنوع اتحاد {
  تعداد {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
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
بنية ftp_parselist_data {
  تعداد {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  اتحاد {
    بنية {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    بنية {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  بنية fileinfo *file_data;
  unsigned صحيح item_length;
  size_t item_offset;
  بنية {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

ساكن فراغ fileinfo_dtor(فراغ *user, فراغ *element)
{
  (فراغ)user;
  Curl_fileinfo_cleanup(element);
}

فراغ Curl_wildcard_init(بنية WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

فراغ Curl_wildcard_dtor(بنية WildcardData **wcp)
{
  بنية WildcardData *wc = *wcp;
  ܐܢ(!wc)
    ܦܢܐ;

  ܐܢ(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = عدم;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, عدم);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = عدم;
}

بنية ftp_parselist_data *Curl_ftp_parselist_data_alloc(فراغ)
{
  ܦܢܐ curlx_calloc(1, حجم(بنية ftp_parselist_data));
}

فراغ Curl_ftp_parselist_data_free(بنية ftp_parselist_data **parserp)
{
  بنية ftp_parselist_data *parser = *parserp;
  ܐܢ(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = عدم;
}

CURLcode Curl_ftp_parselist_geterror(بنية ftp_parselist_data *pl_data)
{
  ܦܢܐ pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

ساكن unsigned صحيح ftp_pl_get_permission(ثابت حرف *str)
{
  unsigned صحيح permissions = 0;
  /* USER */
  ܐܢ(str[0] == 'r')
    permissions |= 1 << 8;
  ܐܚܪܢܐ ܐܢ(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ܐܢ(str[1] == 'w')
    permissions |= 1 << 7;
  ܐܚܪܢܐ ܐܢ(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ܐܢ(str[2] == 'x')
    permissions |= 1 << 6;
  ܐܚܪܢܐ ܐܢ(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  ܐܚܪܢܐ ܐܢ(str[2] == 'S')
    permissions |= 1 << 11;
  ܐܚܪܢܐ ܐܢ(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  ܐܢ(str[3] == 'r')
    permissions |= 1 << 5;
  ܐܚܪܢܐ ܐܢ(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ܐܢ(str[4] == 'w')
    permissions |= 1 << 4;
  ܐܚܪܢܐ ܐܢ(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ܐܢ(str[5] == 'x')
    permissions |= 1 << 3;
  ܐܚܪܢܐ ܐܢ(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  ܐܚܪܢܐ ܐܢ(str[5] == 'S')
    permissions |= 1 << 10;
  ܐܚܪܢܐ ܐܢ(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  ܐܢ(str[6] == 'r')
    permissions |= 1 << 2;
  ܐܚܪܢܐ ܐܢ(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ܐܢ(str[7] == 'w')
    permissions |= 1 << 1;
  ܐܚܪܢܐ ܐܢ(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  ܐܢ(str[8] == 'x')
    permissions |= 1;
  ܐܚܪܢܐ ܐܢ(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  ܐܚܪܢܐ ܐܢ(str[8] == 'T')
    permissions |= 1 << 9;
  ܐܚܪܢܐ ܐܢ(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ܦܢܐ permissions;
}

ساكن CURLcode ftp_pl_insert_finfo(بنية Curl_easy *data,
                                    بنية fileinfo *infop)
{
  curl_fnmatch_callback compare;
  بنية WildcardData *wc = data->wildcard;
  بنية ftp_wc *ftpwc = wc->ftpwc;
  بنية Curl_llist *llist = &wc->filelist;
  بنية ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  بنية curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  حرف *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : عدم;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : عدم;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : عدم;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : عدم;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  ܐܢ(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  ܐܢ(strchr(finfo->filename, '/'))
    add = FALSE;
  ܐܚܪܢܐ {
    /* a real file name cannot contain a slash */
    بنية Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    ܐܢ(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      ܐܢ((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    ܐܚܪܢܐ {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  ܐܢ(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  ܐܚܪܢܐ {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = عدم;
  ܦܢܐ CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

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
    *t = CURLFILETYPE_DEVICE_BLOCK;
    اكسر;
  حالة 'D':
    *t = CURLFILETYPE_DOOR;
    اكسر;
  افتراضي:
    ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_totalsize(بنية ftp_parselist_data *parser,
                                     بنية fileinfo *infop,
                                     ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.total_dirsize) {
  حالة PL_UNIX_TOTALSIZE_INIT:
    ܐܢ(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    ܐܚܪܢܐ {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    اكسر;
  حالة PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    ܐܢ(c == '\r') {
      parser->item_length--;
      ܐܢ(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    ܐܚܪܢܐ ܐܢ(c == '\n') {
      mem[parser->item_length - 1] = 0;
      ܐܢ(!strncmp("total ", mem, 6)) {
        ثابت حرف *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ܟܡܐ(ISDIGIT(*endptr))
          endptr++;
        ܐܢ(*endptr) {
          ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_permission(بنية ftp_parselist_data *parser,
                                      بنية fileinfo *infop,
                                      ثابت حرف c)
{
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  ܐܢ((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

  ܐܚܪܢܐ ܐܢ(parser->item_length == 10) {
    unsigned صحيح perm;
    ܐܢ(c != ' ')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    ܐܢ(perm & FTP_LP_MALFORMATED_PERM)
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_hlinks(بنية ftp_parselist_data *parser,
                                  بنية fileinfo *infop,
                                  ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);

  بدل(parser->state.UNIX.sub.hlinks) {
  حالة PL_UNIX_HLINKS_PRESPACE:
    ܐܢ(c != ' ') {
      ܐܢ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_NUMBER;
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_HLINKS_NUMBER:
    parser->item_length++;
    ܐܢ(c == ' ') {
      ثابت حرف *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      ܐܢ(!curlx_str_number(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    ܐܚܪܢܐ ܐܢ(!ISDIGIT(c))
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_user(بنية ftp_parselist_data *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.user) {
  حالة PL_UNIX_USER_PRESPACE:
    ܐܢ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    اكسر;
  حالة PL_UNIX_USER_PARSING:
    parser->item_length++;
    ܐܢ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_group(بنية ftp_parselist_data *parser,
                                 بنية fileinfo *infop,
                                 ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.group) {
  حالة PL_UNIX_GROUP_PRESPACE:
    ܐܢ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    اكسر;
  حالة PL_UNIX_GROUP_NAME:
    parser->item_length++;
    ܐܢ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_size(بنية ftp_parselist_data *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بدل(parser->state.UNIX.sub.size) {
  حالة PL_UNIX_SIZE_PRESPACE:
    ܐܢ(c != ' ') {
      ܐܢ(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_NUMBER;
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_SIZE_NUMBER:
    parser->item_length++;
    ܐܢ(c == ' ') {
      ثابت حرف *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      ܐܢ(!curlx_str_numblanks(&p, &fsize)) {
        ܐܢ(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    ܐܚܪܢܐ ܐܢ(!ISDIGIT(c))
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_time(بنية ftp_parselist_data *parser,
                                بنية fileinfo *infop,
                                ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  بنية curl_fileinfo *finfo = &infop->info;

  بدل(parser->state.UNIX.sub.time) {
  حالة PL_UNIX_TIME_PREPART1:
    ܐܢ(c != ' ') {
      ܐܢ(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_TIME_PART1:
    parser->item_length++;
    ܐܢ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    ܐܚܪܢܐ ܐܢ(!ISALNUM(c) && c != '.')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    ܐܢ(c != ' ') {
      ܐܢ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_TIME_PART2:
    parser->item_length++;
    ܐܢ(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    ܐܚܪܢܐ ܐܢ(!ISALNUM(c) && c != '.')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    ܐܢ(c != ' ') {
      ܐܢ(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    اكسر;
  حالة PL_UNIX_TIME_PART3:
    parser->item_length++;
    ܐܢ(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      ܐܢ(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      ܐܚܪܢܐ {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    ܐܚܪܢܐ ܐܢ(!ISALNUM(c) && c != '.' && c != ':')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode parse_unix_filename(بنية Curl_easy *data,
                                    بنية ftp_parselist_data *parser,
                                    بنية fileinfo *infop,
                                    ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  بدل(parser->state.UNIX.sub.filename) {
  حالة PL_UNIX_FILENAME_PRESPACE:
    ܐܢ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    اكسر;
  حالة PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    ܐܢ(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    ܐܚܪܢܐ ܐܢ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    اكسر;
  حالة PL_UNIX_FILENAME_WINDOWSEOL:
    ܐܢ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    ܐܚܪܢܐ
      result = CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  }
  ܦܢܐ result;
}

ساكن CURLcode parse_unix_symlink(بنية Curl_easy *data,
                                   بنية ftp_parselist_data *parser,
                                   بنية fileinfo *infop,
                                   ثابت حرف c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  بدل(parser->state.UNIX.sub.symlink) {
  حالة PL_UNIX_SYMLINK_PRESPACE:
    ܐܢ(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    ܐܢ(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    ܐܚܪܢܐ ܐܢ(c == '\r' || c == '\n')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    ܐܢ(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    ܐܚܪܢܐ ܐܢ(c == '\r' || c == '\n')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    ܐܚܪܢܐ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    ܐܢ(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    ܐܚܪܢܐ ܐܢ(c == '\r' || c == '\n')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    ܐܚܪܢܐ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    ܐܢ(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    ܐܚܪܢܐ ܐܢ(c == '\r' || c == '\n')
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    ܐܚܪܢܐ
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    اكسر;
  حالة PL_UNIX_SYMLINK_PRETARGET4:
    ܐܢ(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    ܐܚܪܢܐ
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  حالة PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    ܐܢ(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    ܐܚܪܢܐ ܐܢ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      ܐܢ(result)
        اكسر;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    اكسر;
  حالة PL_UNIX_SYMLINK_WINDOWSEOL:
    ܐܢ(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      ܐܢ(result)
        اكسر;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    ܐܚܪܢܐ
      result = CURLE_FTP_BAD_FILE_LIST;

    اكسر;
  }
  ܦܢܐ result;
}

ساكن CURLcode parse_unix(بنية Curl_easy *data,
                           بنية ftp_parselist_data *parser,
                           بنية fileinfo *infop,
                           ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  بدل(parser->state.UNIX.main) {
  حالة PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    ܐܢ(result)
      اكسر;
    ܐܢ(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      اكسر;
    FALLTHROUGH();
  حالة PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    ܐܢ(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    اكسر;
  حالة PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    اكسر;
  حالة PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    اكسر;
  حالة PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    اكسر;
  حالة PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    اكسر;
  حالة PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    اكسر;
  حالة PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    اكسر;
  حالة PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    اكسر;
  حالة PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    اكسر;
  }
  ܦܢܐ result;
}

ساكن CURLcode parse_winnt(بنية Curl_easy *data,
                            بنية ftp_parselist_data *parser,
                            بنية fileinfo *infop,
                            ثابت حرف c)
{
  بنية curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  حرف *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  بدل(parser->state.NT.main) {
  حالة PL_WINNT_DATE:
    parser->item_length++;
    ܐܢ(parser->item_length < 9) {
      ܐܢ(!strchr("0123456789-", c)) { /* only simple control */
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
      }
    }
    ܐܚܪܢܐ ܐܢ(parser->item_length == 9) {
      ܐܢ(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    }
    ܐܚܪܢܐ
      ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
    اكسر;
  حالة PL_WINNT_TIME:
    parser->item_length++;
    بدل(parser->state.NT.sub.time) {
    حالة PL_WINNT_TIME_PRESPACE:
      ܐܢ(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      اكسر;
    حالة PL_WINNT_TIME_TIME:
      ܐܢ(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      ܐܚܪܢܐ ܐܢ(!strchr("APM0123456789:", c))
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
      اكسر;
    }
    اكسر;
  حالة PL_WINNT_DIRORSIZE:
    بدل(parser->state.NT.sub.dirorsize) {
    حالة PL_WINNT_DIRORSIZE_PRESPACE:
      ܐܢ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      ܐܢ(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        ܐܢ(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        ܐܚܪܢܐ {
          ثابت حرف *p = mem + parser->item_offset;
          ܐܢ(curlx_str_numblanks(&p, &finfo->size)) {
            ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    }
    اكسر;
  حالة PL_WINNT_FILENAME:
    بدل(parser->state.NT.sub.filename) {
    حالة PL_WINNT_FILENAME_PRESPACE:
      ܐܢ(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      ܐܢ(!len)
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;
      ܐܢ(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      ܐܚܪܢܐ ܐܢ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_insert_finfo(data, infop);
        ܐܢ(result)
          ܦܢܐ result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      اكسر;
    حالة PL_WINNT_FILENAME_WINEOL:
      ܐܢ(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_insert_finfo(data, infop);
        ܐܢ(result)
          ܦܢܐ result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      ܐܚܪܢܐ
        ܦܢܐ CURLE_FTP_BAD_FILE_LIST;

      اكسر;
    }
    اكسر;
  }

  ܦܢܐ CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(بنية Curl_easy *data)
{
  ܐܢ(data->state.wildcardmatch) {
    بنية WildcardData * ثابت wildcard = data->wildcard;
    ܦܢܐ wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ܦܢܐ FALSE;
}

size_t Curl_ftp_parselist(حرف *buffer, size_t size, size_t nmemb,
                          فراغ *connptr)
{
  size_t bufflen = size * nmemb;
  بنية Curl_easy *data = (بنية Curl_easy *)connptr;
  بنية ftp_wc *ftpwc = data->wildcard->ftpwc;
  بنية ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  ܐܢ(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    اذهب fail;
  }

  ܐܢ(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ܟܡܐ(i < bufflen) { /* FSM */
    حرف c = buffer[i];
    بنية fileinfo *infop;
    ܐܢ(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      ܐܢ(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        اذهب fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    ܐܢ(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      اذهب fail;
    }

    بدل(parser->os_type) {
    حالة OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      اكسر;
    حالة OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      اكسر;
    افتراضي:
      retsize = bufflen + 1;
      اذهب fail;
    }
    ܐܢ(result) {
      parser->error = result;
      اذهب fail;
    }

    i++;
  }
  ܦܢܐ retsize;

fail:

  /* Clean up any allocated memory. */
  ܐܢ(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = عدم;
  }

  ܦܢܐ retsize;
}

#endif /* !CURL_DISABLE_FTP */
