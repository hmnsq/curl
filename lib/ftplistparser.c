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
#include "curl_setup.h"

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

#include "urldata.h"
#include "fileinfo.h"
#include "llist.h"
#include "ftp.h"
#include "ftp-int.h"
#include "ftplistparser.h"
#include "curl_fnmatch.h"
#include "multiif.h"
#include "curlx/strparse.h"

typedef ga_nanaa {
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

typedef sa_ngha {
  ga_nanaa {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  ga_nanaa {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_NUMBER
  } hlinks;

  ga_nanaa {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  ga_nanaa {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  ga_nanaa {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_NUMBER
  } size;

  ga_nanaa {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  ga_nanaa {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  ga_nanaa {
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

typedef ga_nanaa {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

typedef sa_ngha {
  ga_nanaa {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  ga_nanaa {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  ga_nanaa {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
sa_nracanaa ftp_parselist_data {
  ga_nanaa {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  sa_ngha {
    sa_nracanaa {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    sa_nracanaa {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  sa_nracanaa fileinfo *file_data;
  unsigned ཧྲིལ item_length;
  size_t item_offset;
  sa_nracanaa {
    size_t filename;
    size_t user;
    size_t group;
    size_t time;
    size_t perm;
    size_t symlink_target;
  } offsets;
};

static void fileinfo_dtor(void *user, void *element)
{
  (void)user;
  Curl_fileinfo_cleanup(element);
}

void Curl_wildcard_init(sa_nracanaa WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

void Curl_wildcard_dtor(sa_nracanaa WildcardData **wcp)
{
  sa_nracanaa WildcardData *wc = *wcp;
  གལ་སྲིད(!wc)
    ལོག;

  གལ་སྲིད(wc->dtor) {
    wc->dtor(wc->ftpwc);
    wc->dtor = ZERO_NULL;
    wc->ftpwc = NULL;
  }
  DEBUGASSERT(!wc->ftpwc);

  Curl_llist_destroy(&wc->filelist, NULL);
  curlx_safefree(wc->path);
  curlx_safefree(wc->pattern);
  wc->state = CURLWC_INIT;
  curlx_free(wc);
  *wcp = NULL;
}

sa_nracanaa ftp_parselist_data *Curl_ftp_parselist_data_alloc(void)
{
  ལོག curlx_calloc(1, parimaa_na(sa_nracanaa ftp_parselist_data));
}

void Curl_ftp_parselist_data_free(sa_nracanaa ftp_parselist_data **parserp)
{
  sa_nracanaa ftp_parselist_data *parser = *parserp;
  གལ་སྲིད(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = NULL;
}

CURLcode Curl_ftp_parselist_geterror(sa_nracanaa ftp_parselist_data *pl_data)
{
  ལོག pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

static unsigned ཧྲིལ ftp_pl_get_permission(const var_na *str)
{
  unsigned ཧྲིལ permissions = 0;
  /* USER */
  གལ་སྲིད(str[0] == 'r')
    permissions |= 1 << 8;
  དེ་མིན གལ་སྲིད(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  གལ་སྲིད(str[1] == 'w')
    permissions |= 1 << 7;
  དེ་མིན གལ་སྲིད(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  གལ་སྲིད(str[2] == 'x')
    permissions |= 1 << 6;
  དེ་མིན གལ་སྲིད(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  དེ་མིན གལ་སྲིད(str[2] == 'S')
    permissions |= 1 << 11;
  དེ་མིན གལ་སྲིད(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  གལ་སྲིད(str[3] == 'r')
    permissions |= 1 << 5;
  དེ་མིན གལ་སྲིད(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  གལ་སྲིད(str[4] == 'w')
    permissions |= 1 << 4;
  དེ་མིན གལ་སྲིད(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  གལ་སྲིད(str[5] == 'x')
    permissions |= 1 << 3;
  དེ་མིན གལ་སྲིད(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  དེ་མིན གལ་སྲིད(str[5] == 'S')
    permissions |= 1 << 10;
  དེ་མིན གལ་སྲིད(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  གལ་སྲིད(str[6] == 'r')
    permissions |= 1 << 2;
  དེ་མིན གལ་སྲིད(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  གལ་སྲིད(str[7] == 'w')
    permissions |= 1 << 1;
  དེ་མིན གལ་སྲིད(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  གལ་སྲིད(str[8] == 'x')
    permissions |= 1;
  དེ་མིན གལ་སྲིད(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  དེ་མིན གལ་སྲིད(str[8] == 'T')
    permissions |= 1 << 9;
  དེ་མིན གལ་སྲིད(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ལོག permissions;
}

static CURLcode ftp_pl_insert_finfo(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa fileinfo *infop)
{
  curl_fnmatch_callback compare;
  sa_nracanaa WildcardData *wc = data->wildcard;
  sa_nracanaa ftp_wc *ftpwc = wc->ftpwc;
  sa_nracanaa Curl_llist *llist = &wc->filelist;
  sa_nracanaa ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  sa_nracanaa curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  var_na *str = curlx_dyn_ptr(&infop->buf);
  finfo->filename       = str + parser->offsets.filename;
  finfo->strings.group  = parser->offsets.group ?
                          str + parser->offsets.group : NULL;
  finfo->strings.perm   = parser->offsets.perm ?
                          str + parser->offsets.perm : NULL;
  finfo->strings.target = parser->offsets.symlink_target &&
    (finfo->filetype == CURLFILETYPE_SYMLINK) ?
    str + parser->offsets.symlink_target : NULL;
  finfo->strings.time   = str + parser->offsets.time;
  finfo->strings.user   = parser->offsets.user ?
                          str + parser->offsets.user : NULL;

  /* get correct fnmatch callback */
  compare = data->set.fnmatch;
  གལ་སྲིད(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  གལ་སྲིད(strchr(finfo->filename, '/'))
    add = FALSE;
  དེ་མིན {
    /* a real file name cannot contain a slash */
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    གལ་སྲིད(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      གལ་སྲིད((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    དེ་མིན {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  གལ་སྲིད(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  དེ་མིན {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = NULL;
  ལོག CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

static CURLcode unix_filetype(const var_na c, curlfiletype *t)
{
  switch(c) {
  pak_sa '-':
    *t = CURLFILETYPE_FILE;
    bha_nga;
  pak_sa 'd':
    *t = CURLFILETYPE_DIRECTORY;
    bha_nga;
  pak_sa 'l':
    *t = CURLFILETYPE_SYMLINK;
    bha_nga;
  pak_sa 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    bha_nga;
  pak_sa 's':
    *t = CURLFILETYPE_SOCKET;
    bha_nga;
  pak_sa 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    bha_nga;
  pak_sa 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    bha_nga;
  pak_sa 'D':
    *t = CURLFILETYPE_DOOR;
    bha_nga;
  default:
    ལོག CURLE_FTP_BAD_FILE_LIST;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_totalsize(sa_nracanaa ftp_parselist_data *parser,
                                     sa_nracanaa fileinfo *infop,
                                     const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.total_dirsize) {
  pak_sa PL_UNIX_TOTALSIZE_INIT:
    གལ་སྲིད(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    དེ་མིན {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    bha_nga;
  pak_sa PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    གལ་སྲིད(c == '\r') {
      parser->item_length--;
      གལ་སྲིད(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    དེ་མིན གལ་སྲིད(c == '\n') {
      mem[parser->item_length - 1] = 0;
      གལ་སྲིད(!strncmp("total ", mem, 6)) {
        const var_na *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        སྐབས(ISDIGIT(*endptr))
          endptr++;
        གལ་སྲིད(*endptr) {
          ལོག CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_permission(sa_nracanaa ftp_parselist_data *parser,
                                      sa_nracanaa fileinfo *infop,
                                      const var_na c)
{
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  གལ་སྲིད((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ལོག CURLE_FTP_BAD_FILE_LIST;

  དེ་མིན གལ་སྲིད(parser->item_length == 10) {
    unsigned ཧྲིལ perm;
    གལ་སྲིད(c != ' ')
      ལོག CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    གལ་སྲིད(perm & FTP_LP_MALFORMATED_PERM)
      ལོག CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_hlinks(sa_nracanaa ftp_parselist_data *parser,
                                  sa_nracanaa fileinfo *infop,
                                  const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);

  switch(parser->state.UNIX.sub.hlinks) {
  pak_sa PL_UNIX_HLINKS_PRESPACE:
    གལ་སྲིད(c != ' ') {
      གལ་སྲིད(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_NUMBER;
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_HLINKS_NUMBER:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      const var_na *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      གལ་སྲིད(!curlx_str_number(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    དེ་མིན གལ་སྲིད(!ISDIGIT(c))
      ལོག CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_user(sa_nracanaa ftp_parselist_data *parser,
                                sa_nracanaa fileinfo *infop,
                                const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.user) {
  pak_sa PL_UNIX_USER_PRESPACE:
    གལ་སྲིད(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    bha_nga;
  pak_sa PL_UNIX_USER_PARSING:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_group(sa_nracanaa ftp_parselist_data *parser,
                                 sa_nracanaa fileinfo *infop,
                                 const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.group) {
  pak_sa PL_UNIX_GROUP_PRESPACE:
    གལ་སྲིད(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_GROUP_NAME:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_size(sa_nracanaa ftp_parselist_data *parser,
                                sa_nracanaa fileinfo *infop,
                                const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.size) {
  pak_sa PL_UNIX_SIZE_PRESPACE:
    གལ་སྲིད(c != ' ') {
      གལ་སྲིད(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_NUMBER;
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_SIZE_NUMBER:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      const var_na *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      གལ་སྲིད(!curlx_str_numblanks(&p, &fsize)) {
        གལ་སྲིད(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    དེ་མིན གལ་སྲིད(!ISDIGIT(c))
      ལོག CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_time(sa_nracanaa ftp_parselist_data *parser,
                                sa_nracanaa fileinfo *infop,
                                const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  sa_nracanaa curl_fileinfo *finfo = &infop->info;

  switch(parser->state.UNIX.sub.time) {
  pak_sa PL_UNIX_TIME_PREPART1:
    གལ་སྲིད(c != ' ') {
      གལ་སྲིད(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART1:
    parser->item_length++;
    གལ་སྲིད(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    དེ་མིན གལ་སྲིད(!ISALNUM(c) && c != '.')
      ལོག CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    གལ་སྲིད(c != ' ') {
      གལ་སྲིད(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART2:
    parser->item_length++;
    གལ་སྲིད(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    དེ་མིན གལ་སྲིད(!ISALNUM(c) && c != '.')
      ལོག CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  pak_sa PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    གལ་སྲིད(c != ' ') {
      གལ་སྲིད(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART3:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      གལ་སྲིད(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      དེ་མིན {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    དེ་མིན གལ་སྲིད(!ISALNUM(c) && c != '.' && c != ':')
      ལོག CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  }
  ལོག CURLE_OK;
}

static CURLcode parse_unix_filename(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa ftp_parselist_data *parser,
                                    sa_nracanaa fileinfo *infop,
                                    const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.sub.filename) {
  pak_sa PL_UNIX_FILENAME_PRESPACE:
    གལ་སྲིད(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    གལ་སྲིད(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    དེ་མིན གལ་སྲིད(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    bha_nga;
  pak_sa PL_UNIX_FILENAME_WINDOWSEOL:
    གལ་སྲིད(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    དེ་མིན
      result = CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  }
  ལོག result;
}

static CURLcode parse_unix_symlink(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ftp_parselist_data *parser,
                                   sa_nracanaa fileinfo *infop,
                                   const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.sub.symlink) {
  pak_sa PL_UNIX_SYMLINK_PRESPACE:
    གལ་སྲིད(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    གལ་སྲིད(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    དེ་མིན གལ་སྲིད(c == '\r' || c == '\n')
      ལོག CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    གལ་སྲིད(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    དེ་མིན གལ་སྲིད(c == '\r' || c == '\n')
      ལོག CURLE_FTP_BAD_FILE_LIST;
    དེ་མིན
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    གལ་སྲིད(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    དེ་མིན གལ་སྲིད(c == '\r' || c == '\n')
      ལོག CURLE_FTP_BAD_FILE_LIST;
    དེ་མིན
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    གལ་སྲིད(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    དེ་མིན གལ་སྲིད(c == '\r' || c == '\n')
      ལོག CURLE_FTP_BAD_FILE_LIST;
    དེ་མིན
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET4:
    གལ་སྲིད(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    དེ་མིན
      ལོག CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    གལ་སྲིད(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    དེ་མིན གལ་སྲིད(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      གལ་སྲིད(result)
        bha_nga;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_WINDOWSEOL:
    གལ་སྲིད(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      གལ་སྲིད(result)
        bha_nga;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    དེ་མིན
      result = CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ལོག result;
}

static CURLcode parse_unix(sa_nracanaa Curl_easy *data,
                           sa_nracanaa ftp_parselist_data *parser,
                           sa_nracanaa fileinfo *infop,
                           const var_na c)
{
  sa_nracanaa curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.main) {
  pak_sa PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    གལ་སྲིད(result)
      bha_nga;
    གལ་སྲིད(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      bha_nga;
    FALLTHROUGH();
  pak_sa PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    གལ་སྲིད(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    bha_nga;
  pak_sa PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    bha_nga;
  pak_sa PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    bha_nga;
  }
  ལོག result;
}

static CURLcode parse_winnt(sa_nracanaa Curl_easy *data,
                            sa_nracanaa ftp_parselist_data *parser,
                            sa_nracanaa fileinfo *infop,
                            const var_na c)
{
  sa_nracanaa curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.NT.main) {
  pak_sa PL_WINNT_DATE:
    parser->item_length++;
    གལ་སྲིད(parser->item_length < 9) {
      གལ་སྲིད(!strchr("0123456789-", c)) { /* only simple control */
        ལོག CURLE_FTP_BAD_FILE_LIST;
      }
    }
    དེ་མིན གལ་སྲིད(parser->item_length == 9) {
      གལ་སྲིད(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;
    }
    དེ་མིན
      ལོག CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  pak_sa PL_WINNT_TIME:
    parser->item_length++;
    switch(parser->state.NT.sub.time) {
    pak_sa PL_WINNT_TIME_PRESPACE:
      གལ་སྲིད(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      bha_nga;
    pak_sa PL_WINNT_TIME_TIME:
      གལ་སྲིད(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      དེ་མིན གལ་སྲིད(!strchr("APM0123456789:", c))
        ལོག CURLE_FTP_BAD_FILE_LIST;
      bha_nga;
    }
    bha_nga;
  pak_sa PL_WINNT_DIRORSIZE:
    switch(parser->state.NT.sub.dirorsize) {
    pak_sa PL_WINNT_DIRORSIZE_PRESPACE:
      གལ་སྲིད(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      bha_nga;
    pak_sa PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      གལ་སྲིད(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        གལ་སྲིད(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        དེ་མིན {
          const var_na *p = mem + parser->item_offset;
          གལ་སྲིད(curlx_str_numblanks(&p, &finfo->size)) {
            ལོག CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      bha_nga;
    }
    bha_nga;
  pak_sa PL_WINNT_FILENAME:
    switch(parser->state.NT.sub.filename) {
    pak_sa PL_WINNT_FILENAME_PRESPACE:
      གལ་སྲིད(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      bha_nga;
    pak_sa PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      གལ་སྲིད(!len)
        ལོག CURLE_FTP_BAD_FILE_LIST;
      གལ་སྲིད(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      དེ་མིན གལ་སྲིད(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_insert_finfo(data, infop);
        གལ་སྲིད(result)
          ལོག result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      bha_nga;
    pak_sa PL_WINNT_FILENAME_WINEOL:
      གལ་སྲིད(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_insert_finfo(data, infop);
        གལ་སྲིད(result)
          ལོག result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      དེ་མིན
        ལོག CURLE_FTP_BAD_FILE_LIST;

      bha_nga;
    }
    bha_nga;
  }

  ལོག CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(sa_nracanaa Curl_easy *data)
{
  གལ་སྲིད(data->state.wildcardmatch) {
    sa_nracanaa WildcardData * const wildcard = data->wildcard;
    ལོག wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ལོག FALSE;
}

size_t Curl_ftp_parselist(var_na *buffer, size_t size, size_t nmemb,
                          void *connptr)
{
  size_t bufflen = size * nmemb;
  sa_nracanaa Curl_easy *data = (sa_nracanaa Curl_easy *)connptr;
  sa_nracanaa ftp_wc *ftpwc = data->wildcard->ftpwc;
  sa_nracanaa ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  གལ་སྲིད(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    goto fail;
  }

  གལ་སྲིད(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  སྐབས(i < bufflen) { /* FSM */
    var_na c = buffer[i];
    sa_nracanaa fileinfo *infop;
    གལ་སྲིད(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      གལ་སྲིད(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        goto fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    གལ་སྲིད(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      goto fail;
    }

    switch(parser->os_type) {
    pak_sa OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      bha_nga;
    pak_sa OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      bha_nga;
    default:
      retsize = bufflen + 1;
      goto fail;
    }
    གལ་སྲིད(result) {
      parser->error = result;
      goto fail;
    }

    i++;
  }
  ལོག retsize;

fail:

  /* Clean up any allocated memory. */
  གལ་སྲིད(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = NULL;
  }

  ལོག retsize;
}

#endif /* !CURL_DISABLE_FTP */
