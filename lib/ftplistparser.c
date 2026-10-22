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
  unsigned ကိန်းပြည့် item_length;
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
  အကယ်၍(!wc)
    ပြန်ပေး;

  အကယ်၍(wc->dtor) {
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
  ပြန်ပေး curlx_calloc(1, parimaa_na(sa_nracanaa ftp_parselist_data));
}

void Curl_ftp_parselist_data_free(sa_nracanaa ftp_parselist_data **parserp)
{
  sa_nracanaa ftp_parselist_data *parser = *parserp;
  အကယ်၍(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = NULL;
}

CURLcode Curl_ftp_parselist_geterror(sa_nracanaa ftp_parselist_data *pl_data)
{
  ပြန်ပေး pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

static unsigned ကိန်းပြည့် ftp_pl_get_permission(const var_na *str)
{
  unsigned ကိန်းပြည့် permissions = 0;
  /* USER */
  အကယ်၍(str[0] == 'r')
    permissions |= 1 << 8;
  မဟုတ်ပါက အကယ်၍(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  အကယ်၍(str[1] == 'w')
    permissions |= 1 << 7;
  မဟုတ်ပါက အကယ်၍(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  အကယ်၍(str[2] == 'x')
    permissions |= 1 << 6;
  မဟုတ်ပါက အကယ်၍(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  မဟုတ်ပါက အကယ်၍(str[2] == 'S')
    permissions |= 1 << 11;
  မဟုတ်ပါက အကယ်၍(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  အကယ်၍(str[3] == 'r')
    permissions |= 1 << 5;
  မဟုတ်ပါက အကယ်၍(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  အကယ်၍(str[4] == 'w')
    permissions |= 1 << 4;
  မဟုတ်ပါက အကယ်၍(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  အကယ်၍(str[5] == 'x')
    permissions |= 1 << 3;
  မဟုတ်ပါက အကယ်၍(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  မဟုတ်ပါက အကယ်၍(str[5] == 'S')
    permissions |= 1 << 10;
  မဟုတ်ပါက အကယ်၍(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  အကယ်၍(str[6] == 'r')
    permissions |= 1 << 2;
  မဟုတ်ပါက အကယ်၍(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  အကယ်၍(str[7] == 'w')
    permissions |= 1 << 1;
  မဟုတ်ပါက အကယ်၍(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  အကယ်၍(str[8] == 'x')
    permissions |= 1;
  မဟုတ်ပါက အကယ်၍(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  မဟုတ်ပါက အကယ်၍(str[8] == 'T')
    permissions |= 1 << 9;
  မဟုတ်ပါက အကယ်၍(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  ပြန်ပေး permissions;
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
  အကယ်၍(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  အကယ်၍(strchr(finfo->filename, '/'))
    add = FALSE;
  မဟုတ်ပါက {
    /* a real file name cannot contain a slash */
    sa_nracanaa Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    အကယ်၍(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      အကယ်၍((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    မဟုတ်ပါက {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  အကယ်၍(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  မဟုတ်ပါက {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = NULL;
  ပြန်ပေး CURLE_OK;
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
    ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_totalsize(sa_nracanaa ftp_parselist_data *parser,
                                     sa_nracanaa fileinfo *infop,
                                     const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.total_dirsize) {
  pak_sa PL_UNIX_TOTALSIZE_INIT:
    အကယ်၍(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    မဟုတ်ပါက {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    bha_nga;
  pak_sa PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    အကယ်၍(c == '\r') {
      parser->item_length--;
      အကယ်၍(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    မဟုတ်ပါက အကယ်၍(c == '\n') {
      mem[parser->item_length - 1] = 0;
      အကယ်၍(!strncmp("total ", mem, 6)) {
        const var_na *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        နေစဉ်(ISDIGIT(*endptr))
          endptr++;
        အကယ်၍(*endptr) {
          ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_permission(sa_nracanaa ftp_parselist_data *parser,
                                      sa_nracanaa fileinfo *infop,
                                      const var_na c)
{
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  အကယ်၍((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

  မဟုတ်ပါက အကယ်၍(parser->item_length == 10) {
    unsigned ကိန်းပြည့် perm;
    အကယ်၍(c != ' ')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    အကယ်၍(perm & FTP_LP_MALFORMATED_PERM)
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_hlinks(sa_nracanaa ftp_parselist_data *parser,
                                  sa_nracanaa fileinfo *infop,
                                  const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);

  switch(parser->state.UNIX.sub.hlinks) {
  pak_sa PL_UNIX_HLINKS_PRESPACE:
    အကယ်၍(c != ' ') {
      အကယ်၍(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_NUMBER;
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_HLINKS_NUMBER:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      const var_na *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      အကယ်၍(!curlx_str_number(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    မဟုတ်ပါက အကယ်၍(!ISDIGIT(c))
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_user(sa_nracanaa ftp_parselist_data *parser,
                                sa_nracanaa fileinfo *infop,
                                const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.user) {
  pak_sa PL_UNIX_USER_PRESPACE:
    အကယ်၍(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    bha_nga;
  pak_sa PL_UNIX_USER_PARSING:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_group(sa_nracanaa ftp_parselist_data *parser,
                                 sa_nracanaa fileinfo *infop,
                                 const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.group) {
  pak_sa PL_UNIX_GROUP_PRESPACE:
    အကယ်၍(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_GROUP_NAME:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
}

static CURLcode parse_unix_size(sa_nracanaa ftp_parselist_data *parser,
                                sa_nracanaa fileinfo *infop,
                                const var_na c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  var_na *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.size) {
  pak_sa PL_UNIX_SIZE_PRESPACE:
    အကယ်၍(c != ' ') {
      အကယ်၍(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_NUMBER;
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_SIZE_NUMBER:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      const var_na *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      အကယ်၍(!curlx_str_numblanks(&p, &fsize)) {
        အကယ်၍(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    မဟုတ်ပါက အကယ်၍(!ISDIGIT(c))
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
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
    အကယ်၍(c != ' ') {
      အကယ်၍(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART1:
    parser->item_length++;
    အကယ်၍(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    မဟုတ်ပါက အကယ်၍(!ISALNUM(c) && c != '.')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    အကယ်၍(c != ' ') {
      အကယ်၍(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART2:
    parser->item_length++;
    အကယ်၍(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    မဟုတ်ပါက အကယ်၍(!ISALNUM(c) && c != '.')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  pak_sa PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    အကယ်၍(c != ' ') {
      အကယ်၍(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    bha_nga;
  pak_sa PL_UNIX_TIME_PART3:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      အကယ်၍(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      မဟုတ်ပါက {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    မဟုတ်ပါက အကယ်၍(!ISALNUM(c) && c != '.' && c != ':')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
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
    အကယ်၍(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    အကယ်၍(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    မဟုတ်ပါက အကယ်၍(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    bha_nga;
  pak_sa PL_UNIX_FILENAME_WINDOWSEOL:
    အကယ်၍(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    မဟုတ်ပါက
      result = CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  }
  ပြန်ပေး result;
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
    အကယ်၍(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    အကယ်၍(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    မဟုတ်ပါက အကယ်၍(c == '\r' || c == '\n')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    အကယ်၍(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    မဟုတ်ပါက အကယ်၍(c == '\r' || c == '\n')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    မဟုတ်ပါက
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    အကယ်၍(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    မဟုတ်ပါက အကယ်၍(c == '\r' || c == '\n')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    မဟုတ်ပါက
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    အကယ်၍(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    မဟုတ်ပါက အကယ်၍(c == '\r' || c == '\n')
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    မဟုတ်ပါက
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_PRETARGET4:
    အကယ်၍(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    မဟုတ်ပါက
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  pak_sa PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    အကယ်၍(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    မဟုတ်ပါက အကယ်၍(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      အကယ်၍(result)
        bha_nga;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    bha_nga;
  pak_sa PL_UNIX_SYMLINK_WINDOWSEOL:
    အကယ်၍(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      အကယ်၍(result)
        bha_nga;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    မဟုတ်ပါက
      result = CURLE_FTP_BAD_FILE_LIST;

    bha_nga;
  }
  ပြန်ပေး result;
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
    အကယ်၍(result)
      bha_nga;
    အကယ်၍(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      bha_nga;
    FALLTHROUGH();
  pak_sa PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    အကယ်၍(!result) {
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
  ပြန်ပေး result;
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
    အကယ်၍(parser->item_length < 9) {
      အကယ်၍(!strchr("0123456789-", c)) { /* only simple control */
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
      }
    }
    မဟုတ်ပါက အကယ်၍(parser->item_length == 9) {
      အကယ်၍(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    }
    မဟုတ်ပါက
      ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
    bha_nga;
  pak_sa PL_WINNT_TIME:
    parser->item_length++;
    switch(parser->state.NT.sub.time) {
    pak_sa PL_WINNT_TIME_PRESPACE:
      အကယ်၍(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      bha_nga;
    pak_sa PL_WINNT_TIME_TIME:
      အကယ်၍(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      မဟုတ်ပါက အကယ်၍(!strchr("APM0123456789:", c))
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
      bha_nga;
    }
    bha_nga;
  pak_sa PL_WINNT_DIRORSIZE:
    switch(parser->state.NT.sub.dirorsize) {
    pak_sa PL_WINNT_DIRORSIZE_PRESPACE:
      အကယ်၍(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      bha_nga;
    pak_sa PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      အကယ်၍(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        အကယ်၍(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        မဟုတ်ပါက {
          const var_na *p = mem + parser->item_offset;
          အကယ်၍(curlx_str_numblanks(&p, &finfo->size)) {
            ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
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
      အကယ်၍(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      bha_nga;
    pak_sa PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      အကယ်၍(!len)
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;
      အကယ်၍(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      မဟုတ်ပါက အကယ်၍(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_insert_finfo(data, infop);
        အကယ်၍(result)
          ပြန်ပေး result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      bha_nga;
    pak_sa PL_WINNT_FILENAME_WINEOL:
      အကယ်၍(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_insert_finfo(data, infop);
        အကယ်၍(result)
          ပြန်ပေး result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      မဟုတ်ပါက
        ပြန်ပေး CURLE_FTP_BAD_FILE_LIST;

      bha_nga;
    }
    bha_nga;
  }

  ပြန်ပေး CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(sa_nracanaa Curl_easy *data)
{
  အကယ်၍(data->state.wildcardmatch) {
    sa_nracanaa WildcardData * const wildcard = data->wildcard;
    ပြန်ပေး wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  ပြန်ပေး FALSE;
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

  အကယ်၍(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    goto fail;
  }

  အကယ်၍(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  နေစဉ်(i < bufflen) { /* FSM */
    var_na c = buffer[i];
    sa_nracanaa fileinfo *infop;
    အကယ်၍(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      အကယ်၍(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        goto fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    အကယ်၍(curlx_dyn_addn(&infop->buf, &c, 1)) {
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
    အကယ်၍(result) {
      parser->error = result;
      goto fail;
    }

    i++;
  }
  ပြန်ပေး retsize;

fail:

  /* Clean up any allocated memory. */
  အကယ်၍(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = NULL;
  }

  ပြန်ပေး retsize;
}

#endif /* !CURL_DISABLE_FTP */
