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

typedef enum {
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

typedef union {
  enum {
    PL_UNIX_TOTALSIZE_INIT = 0,
    PL_UNIX_TOTALSIZE_READING
  } total_dirsize;

  enum {
    PL_UNIX_HLINKS_PRESPACE = 0,
    PL_UNIX_HLINKS_NUMBER
  } hlinks;

  enum {
    PL_UNIX_USER_PRESPACE = 0,
    PL_UNIX_USER_PARSING
  } user;

  enum {
    PL_UNIX_GROUP_PRESPACE = 0,
    PL_UNIX_GROUP_NAME
  } group;

  enum {
    PL_UNIX_SIZE_PRESPACE = 0,
    PL_UNIX_SIZE_NUMBER
  } size;

  enum {
    PL_UNIX_TIME_PREPART1 = 0,
    PL_UNIX_TIME_PART1,
    PL_UNIX_TIME_PREPART2,
    PL_UNIX_TIME_PART2,
    PL_UNIX_TIME_PREPART3,
    PL_UNIX_TIME_PART3
  } time;

  enum {
    PL_UNIX_FILENAME_PRESPACE = 0,
    PL_UNIX_FILENAME_NAME,
    PL_UNIX_FILENAME_WINDOWSEOL
  } filename;

  enum {
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

typedef enum {
  PL_WINNT_DATE = 0,
  PL_WINNT_TIME,
  PL_WINNT_DIRORSIZE,
  PL_WINNT_FILENAME
} pl_winNT_mainstate;

typedef union {
  enum {
    PL_WINNT_TIME_PRESPACE = 0,
    PL_WINNT_TIME_TIME
  } time;
  enum {
    PL_WINNT_DIRORSIZE_PRESPACE = 0,
    PL_WINNT_DIRORSIZE_CONTENT
  } dirorsize;
  enum {
    PL_WINNT_FILENAME_PRESPACE = 0,
    PL_WINNT_FILENAME_CONTENT,
    PL_WINNT_FILENAME_WINEOL
  } filename;
} pl_winNT_substate;

/* This struct is used in wildcard downloading - for parsing LIST response */
struct ftp_parselist_data {
  enum {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_UNIX,
    OS_TYPE_WIN_NT
  } os_type;

  union {
    struct {
      pl_unix_mainstate main;
      pl_unix_substate sub;
    } UNIX;

    struct {
      pl_winNT_mainstate main;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  struct fileinfo *file_data;
  unsigned số_nguyên item_length;
  size_t item_offset;
  struct {
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

void Curl_wildcard_init(struct WildcardData *wc)
{
  Curl_llist_init(&wc->filelist, fileinfo_dtor);
  wc->state = CURLWC_INIT;
}

void Curl_wildcard_dtor(struct WildcardData **wcp)
{
  struct WildcardData *wc = *wcp;
  nếu(!wc)
    trả_về;

  nếu(wc->dtor) {
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

struct ftp_parselist_data *Curl_ftp_parselist_data_alloc(void)
{
  trả_về curlx_calloc(1, sizeof(struct ftp_parselist_data));
}

void Curl_ftp_parselist_data_free(struct ftp_parselist_data **parserp)
{
  struct ftp_parselist_data *parser = *parserp;
  nếu(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = NULL;
}

CURLcode Curl_ftp_parselist_geterror(struct ftp_parselist_data *pl_data)
{
  trả_về pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

static unsigned số_nguyên ftp_pl_get_permission(const char *str)
{
  unsigned số_nguyên permissions = 0;
  /* USER */
  nếu(str[0] == 'r')
    permissions |= 1 << 8;
  không_thì nếu(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  nếu(str[1] == 'w')
    permissions |= 1 << 7;
  không_thì nếu(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  nếu(str[2] == 'x')
    permissions |= 1 << 6;
  không_thì nếu(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  không_thì nếu(str[2] == 'S')
    permissions |= 1 << 11;
  không_thì nếu(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  nếu(str[3] == 'r')
    permissions |= 1 << 5;
  không_thì nếu(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  nếu(str[4] == 'w')
    permissions |= 1 << 4;
  không_thì nếu(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  nếu(str[5] == 'x')
    permissions |= 1 << 3;
  không_thì nếu(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  không_thì nếu(str[5] == 'S')
    permissions |= 1 << 10;
  không_thì nếu(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  nếu(str[6] == 'r')
    permissions |= 1 << 2;
  không_thì nếu(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  nếu(str[7] == 'w')
    permissions |= 1 << 1;
  không_thì nếu(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  nếu(str[8] == 'x')
    permissions |= 1;
  không_thì nếu(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  không_thì nếu(str[8] == 'T')
    permissions |= 1 << 9;
  không_thì nếu(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  trả_về permissions;
}

static CURLcode ftp_pl_insert_finfo(struct Curl_easy *data,
                                    struct fileinfo *infop)
{
  curl_fnmatch_callback compare;
  struct WildcardData *wc = data->wildcard;
  struct ftp_wc *ftpwc = wc->ftpwc;
  struct Curl_llist *llist = &wc->filelist;
  struct ftp_parselist_data *parser = ftpwc->parser;
  bool add = TRUE;
  struct curl_fileinfo *finfo = &infop->info;

  /* set the finfo pointers */
  char *str = curlx_dyn_ptr(&infop->buf);
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
  nếu(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  nếu(strchr(finfo->filename, '/'))
    add = FALSE;
  không_thì {
    /* a real file name cannot contain a slash */
    struct Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    nếu(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      nếu((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    không_thì {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  nếu(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  không_thì {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = NULL;
  trả_về CURLE_OK;
}

#define MAX_FTPLIST_BUFFER 10000 /* arbitrarily set */

static CURLcode unix_filetype(const char c, curlfiletype *t)
{
  switch(c) {
  case '-':
    *t = CURLFILETYPE_FILE;
    break;
  case 'd':
    *t = CURLFILETYPE_DIRECTORY;
    break;
  case 'l':
    *t = CURLFILETYPE_SYMLINK;
    break;
  case 'p':
    *t = CURLFILETYPE_NAMEDPIPE;
    break;
  case 's':
    *t = CURLFILETYPE_SOCKET;
    break;
  case 'c':
    *t = CURLFILETYPE_DEVICE_CHAR;
    break;
  case 'b':
    *t = CURLFILETYPE_DEVICE_BLOCK;
    break;
  case 'D':
    *t = CURLFILETYPE_DOOR;
    break;
  default:
    trả_về CURLE_FTP_BAD_FILE_LIST;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_totalsize(struct ftp_parselist_data *parser,
                                     struct fileinfo *infop,
                                     const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.total_dirsize) {
  case PL_UNIX_TOTALSIZE_INIT:
    nếu(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    không_thì {
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    break;
  case PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    nếu(c == '\r') {
      parser->item_length--;
      nếu(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    không_thì nếu(c == '\n') {
      mem[parser->item_length - 1] = 0;
      nếu(!strncmp("total ", mem, 6)) {
        const char *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        trong_khi(ISDIGIT(*endptr))
          endptr++;
        nếu(*endptr) {
          trả_về CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.main = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_permission(struct ftp_parselist_data *parser,
                                      struct fileinfo *infop,
                                      const char c)
{
  char *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  nếu((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    trả_về CURLE_FTP_BAD_FILE_LIST;

  không_thì nếu(parser->item_length == 10) {
    unsigned số_nguyên perm;
    nếu(c != ' ')
      trả_về CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    nếu(perm & FTP_LP_MALFORMATED_PERM)
      trả_về CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.main = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_hlinks(struct ftp_parselist_data *parser,
                                  struct fileinfo *infop,
                                  const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);

  switch(parser->state.UNIX.sub.hlinks) {
  case PL_UNIX_HLINKS_PRESPACE:
    nếu(c != ' ') {
      nếu(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_NUMBER;
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_HLINKS_NUMBER:
    parser->item_length++;
    nếu(c == ' ') {
      const char *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      nếu(!curlx_str_number(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.main = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    không_thì nếu(!ISDIGIT(c))
      trả_về CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_user(struct ftp_parselist_data *parser,
                                struct fileinfo *infop,
                                const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.user) {
  case PL_UNIX_USER_PRESPACE:
    nếu(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    break;
  case PL_UNIX_USER_PARSING:
    parser->item_length++;
    nếu(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_group(struct ftp_parselist_data *parser,
                                 struct fileinfo *infop,
                                 const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.group) {
  case PL_UNIX_GROUP_PRESPACE:
    nếu(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    break;
  case PL_UNIX_GROUP_NAME:
    parser->item_length++;
    nếu(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_size(struct ftp_parselist_data *parser,
                                struct fileinfo *infop,
                                const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.size) {
  case PL_UNIX_SIZE_PRESPACE:
    nếu(c != ' ') {
      nếu(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_NUMBER;
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_SIZE_NUMBER:
    parser->item_length++;
    nếu(c == ' ') {
      const char *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      nếu(!curlx_str_numblanks(&p, &fsize)) {
        nếu(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.main = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    không_thì nếu(!ISDIGIT(c))
      trả_về CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_time(struct ftp_parselist_data *parser,
                                struct fileinfo *infop,
                                const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  struct curl_fileinfo *finfo = &infop->info;

  switch(parser->state.UNIX.sub.time) {
  case PL_UNIX_TIME_PREPART1:
    nếu(c != ' ') {
      nếu(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART1:
    parser->item_length++;
    nếu(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    không_thì nếu(!ISALNUM(c) && c != '.')
      trả_về CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    nếu(c != ' ') {
      nếu(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART2:
    parser->item_length++;
    nếu(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    không_thì nếu(!ISALNUM(c) && c != '.')
      trả_về CURLE_FTP_BAD_FILE_LIST;
    break;
  case PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    nếu(c != ' ') {
      nếu(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART3:
    parser->item_length++;
    nếu(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      nếu(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.main = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      không_thì {
        parser->state.UNIX.main = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    không_thì nếu(!ISALNUM(c) && c != '.' && c != ':')
      trả_về CURLE_FTP_BAD_FILE_LIST;
    break;
  }
  trả_về CURLE_OK;
}

static CURLcode parse_unix_filename(struct Curl_easy *data,
                                    struct ftp_parselist_data *parser,
                                    struct fileinfo *infop,
                                    const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.sub.filename) {
  case PL_UNIX_FILENAME_PRESPACE:
    nếu(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    break;
  case PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    nếu(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    không_thì nếu(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    break;
  case PL_UNIX_FILENAME_WINDOWSEOL:
    nếu(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.main = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    không_thì
      result = CURLE_FTP_BAD_FILE_LIST;
    break;
  }
  trả_về result;
}

static CURLcode parse_unix_symlink(struct Curl_easy *data,
                                   struct ftp_parselist_data *parser,
                                   struct fileinfo *infop,
                                   const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.sub.symlink) {
  case PL_UNIX_SYMLINK_PRESPACE:
    nếu(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    break;
  case PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    nếu(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    không_thì nếu(c == '\r' || c == '\n')
      trả_về CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    nếu(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    không_thì nếu(c == '\r' || c == '\n')
      trả_về CURLE_FTP_BAD_FILE_LIST;
    không_thì
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    break;
  case PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    nếu(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    không_thì nếu(c == '\r' || c == '\n')
      trả_về CURLE_FTP_BAD_FILE_LIST;
    không_thì
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    break;
  case PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    nếu(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    không_thì nếu(c == '\r' || c == '\n')
      trả_về CURLE_FTP_BAD_FILE_LIST;
    không_thì
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    break;
  case PL_UNIX_SYMLINK_PRETARGET4:
    nếu(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    không_thì
      trả_về CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    nếu(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    không_thì nếu(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      nếu(result)
        break;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    break;
  case PL_UNIX_SYMLINK_WINDOWSEOL:
    nếu(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      nếu(result)
        break;

      parser->state.UNIX.main = PL_UNIX_FILETYPE;
    }
    không_thì
      result = CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  trả_về result;
}

static CURLcode parse_unix(struct Curl_easy *data,
                           struct ftp_parselist_data *parser,
                           struct fileinfo *infop,
                           const char c)
{
  struct curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.main) {
  case PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    nếu(result)
      break;
    nếu(parser->state.UNIX.main != PL_UNIX_FILETYPE)
      break;
    FALLTHROUGH();
  case PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    nếu(!result) {
      parser->state.UNIX.main = PL_UNIX_PERMISSION;
      parser->item_length = 0;
      parser->item_offset = 1;
    }
    break;
  case PL_UNIX_PERMISSION:
    result = parse_unix_permission(parser, infop, c);
    break;
  case PL_UNIX_HLINKS:
    result = parse_unix_hlinks(parser, infop, c);
    break;
  case PL_UNIX_USER:
    result = parse_unix_user(parser, infop, c);
    break;
  case PL_UNIX_GROUP:
    result = parse_unix_group(parser, infop, c);
    break;
  case PL_UNIX_SIZE:
    result = parse_unix_size(parser, infop, c);
    break;
  case PL_UNIX_TIME:
    result = parse_unix_time(parser, infop, c);
    break;
  case PL_UNIX_FILENAME:
    result = parse_unix_filename(data, parser, infop, c);
    break;
  case PL_UNIX_SYMLINK:
    result = parse_unix_symlink(data, parser, infop, c);
    break;
  }
  trả_về result;
}

static CURLcode parse_winnt(struct Curl_easy *data,
                            struct ftp_parselist_data *parser,
                            struct fileinfo *infop,
                            const char c)
{
  struct curl_fileinfo *finfo = &infop->info;
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  CURLcode result = CURLE_OK;

  switch(parser->state.NT.main) {
  case PL_WINNT_DATE:
    parser->item_length++;
    nếu(parser->item_length < 9) {
      nếu(!strchr("0123456789-", c)) { /* only simple control */
        trả_về CURLE_FTP_BAD_FILE_LIST;
      }
    }
    không_thì nếu(parser->item_length == 9) {
      nếu(c == ' ') {
        parser->state.NT.main = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;
    }
    không_thì
      trả_về CURLE_FTP_BAD_FILE_LIST;
    break;
  case PL_WINNT_TIME:
    parser->item_length++;
    switch(parser->state.NT.sub.time) {
    case PL_WINNT_TIME_PRESPACE:
      nếu(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      break;
    case PL_WINNT_TIME_TIME:
      nếu(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.main = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      không_thì nếu(!strchr("APM0123456789:", c))
        trả_về CURLE_FTP_BAD_FILE_LIST;
      break;
    }
    break;
  case PL_WINNT_DIRORSIZE:
    switch(parser->state.NT.sub.dirorsize) {
    case PL_WINNT_DIRORSIZE_PRESPACE:
      nếu(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      break;
    case PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      nếu(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        nếu(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        không_thì {
          const char *p = mem + parser->item_offset;
          nếu(curlx_str_numblanks(&p, &finfo->size)) {
            trả_về CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.main = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      break;
    }
    break;
  case PL_WINNT_FILENAME:
    switch(parser->state.NT.sub.filename) {
    case PL_WINNT_FILENAME_PRESPACE:
      nếu(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      break;
    case PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      nếu(!len)
        trả_về CURLE_FTP_BAD_FILE_LIST;
      nếu(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      không_thì nếu(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_insert_finfo(data, infop);
        nếu(result)
          trả_về result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      break;
    case PL_WINNT_FILENAME_WINEOL:
      nếu(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_insert_finfo(data, infop);
        nếu(result)
          trả_về result;

        parser->state.NT.main = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      không_thì
        trả_về CURLE_FTP_BAD_FILE_LIST;

      break;
    }
    break;
  }

  trả_về CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(struct Curl_easy *data)
{
  nếu(data->state.wildcardmatch) {
    struct WildcardData * const wildcard = data->wildcard;
    trả_về wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  trả_về FALSE;
}

size_t Curl_ftp_parselist(char *buffer, size_t size, size_t nmemb,
                          void *connptr)
{
  size_t bufflen = size * nmemb;
  struct Curl_easy *data = (struct Curl_easy *)connptr;
  struct ftp_wc *ftpwc = data->wildcard->ftpwc;
  struct ftp_parselist_data *parser = ftpwc->parser;
  size_t i = 0;
  CURLcode result;
  size_t retsize = bufflen;

  nếu(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    goto fail;
  }

  nếu(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  trong_khi(i < bufflen) { /* FSM */
    char c = buffer[i];
    struct fileinfo *infop;
    nếu(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      nếu(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        goto fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    nếu(curlx_dyn_addn(&infop->buf, &c, 1)) {
      parser->error = CURLE_OUT_OF_MEMORY;
      goto fail;
    }

    switch(parser->os_type) {
    case OS_TYPE_UNIX:
      result = parse_unix(data, parser, infop, c);
      break;
    case OS_TYPE_WIN_NT:
      result = parse_winnt(data, parser, infop, c);
      break;
    default:
      retsize = bufflen + 1;
      goto fail;
    }
    nếu(result) {
      parser->error = result;
      goto fail;
    }

    i++;
  }
  trả_về retsize;

fail:

  /* Clean up any allocated memory. */
  nếu(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = NULL;
  }

  trả_về retsize;
}

#endif /* !CURL_DISABLE_FTP */
