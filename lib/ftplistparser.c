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
      pl_unix_mainstate メインページ;
      pl_unix_substate sub;
    } UNIX;

    struct {
      pl_winNT_mainstate メインページ;
      pl_winNT_substate sub;
    } NT;
  } state;

  CURLcode error;
  struct fileinfo *file_data;
  unsigned 整数 item_length;
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
  もし(!wc)
    戻る;

  もし(wc->dtor) {
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
  戻る curlx_calloc(1, sizeof(struct ftp_parselist_data));
}

void Curl_ftp_parselist_data_free(struct ftp_parselist_data **parserp)
{
  struct ftp_parselist_data *parser = *parserp;
  もし(parser)
    Curl_fileinfo_cleanup(parser->file_data);
  curlx_free(parser);
  *parserp = NULL;
}

CURLcode Curl_ftp_parselist_geterror(struct ftp_parselist_data *pl_data)
{
  戻る pl_data->error;
}

#define FTP_LP_MALFORMATED_PERM 0x01000000

static unsigned 整数 ftp_pl_get_permission(const char *str)
{
  unsigned 整数 permissions = 0;
  /* USER */
  もし(str[0] == 'r')
    permissions |= 1 << 8;
  否则 もし(str[0] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  もし(str[1] == 'w')
    permissions |= 1 << 7;
  否则 もし(str[1] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  もし(str[2] == 'x')
    permissions |= 1 << 6;
  否则 もし(str[2] == 's') {
    permissions |= 1 << 6;
    permissions |= 1 << 11;
  }
  否则 もし(str[2] == 'S')
    permissions |= 1 << 11;
  否则 もし(str[2] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* GROUP */
  もし(str[3] == 'r')
    permissions |= 1 << 5;
  否则 もし(str[3] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  もし(str[4] == 'w')
    permissions |= 1 << 4;
  否则 もし(str[4] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  もし(str[5] == 'x')
    permissions |= 1 << 3;
  否则 もし(str[5] == 's') {
    permissions |= 1 << 3;
    permissions |= 1 << 10;
  }
  否则 もし(str[5] == 'S')
    permissions |= 1 << 10;
  否则 もし(str[5] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  /* others */
  もし(str[6] == 'r')
    permissions |= 1 << 2;
  否则 もし(str[6] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  もし(str[7] == 'w')
    permissions |= 1 << 1;
  否则 もし(str[7] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;
  もし(str[8] == 'x')
    permissions |= 1;
  否则 もし(str[8] == 't') {
    permissions |= 1;
    permissions |= 1 << 9;
  }
  否则 もし(str[8] == 'T')
    permissions |= 1 << 9;
  否则 もし(str[8] != '-')
    permissions |= FTP_LP_MALFORMATED_PERM;

  戻る permissions;
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
  もし(!compare)
    compare = Curl_fnmatch;

  /* filter pattern-corresponding filenames */
  もし(strchr(finfo->filename, '/'))
    add = FALSE;
  否则 {
    /* a real file name cannot contain a slash */
    struct Curl_mapi_guard guard;
    CURL_CBAPI_START(&guard, data, easy_fnmatch_data);
    もし(compare(data->set.fnmatch_data, wc->pattern, finfo->filename) == 0) {
      /* discard symlink which is containing multiple " -> " */
      もし((finfo->filetype == CURLFILETYPE_SYMLINK) && finfo->strings.target &&
         strstr(finfo->strings.target, " -> ")) {
        add = FALSE;
      }
    }
    否则 {
      add = FALSE;
    }
    CURL_CBAPI_END(&guard);
  }

  もし(add) {
    Curl_llist_append(llist, finfo, &infop->list);
  }
  否则 {
    Curl_fileinfo_cleanup(infop);
  }

  ftpwc->parser->file_data = NULL;
  戻る CURLE_OK;
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
    戻る CURLE_FTP_BAD_FILE_LIST;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_totalsize(struct ftp_parselist_data *parser,
                                     struct fileinfo *infop,
                                     const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.total_dirsize) {
  case PL_UNIX_TOTALSIZE_INIT:
    もし(c == 't') {
      parser->state.UNIX.sub.total_dirsize = PL_UNIX_TOTALSIZE_READING;
      parser->item_length++;
    }
    否则 {
      parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
      /* continue to fall through */
    }
    break;
  case PL_UNIX_TOTALSIZE_READING:
    parser->item_length++;
    もし(c == '\r') {
      parser->item_length--;
      もし(len)
        curlx_dyn_setlen(&infop->buf, --len);
    }
    否则 もし(c == '\n') {
      mem[parser->item_length - 1] = 0;
      もし(!strncmp("total ", mem, 6)) {
        const char *endptr = mem + 6;
        /* here we can deal with directory size, pass the leading
           whitespace and then the digits */
        curlx_str_passblanks(&endptr);
        ながら(ISDIGIT(*endptr))
          endptr++;
        もし(*endptr) {
          戻る CURLE_FTP_BAD_FILE_LIST;
        }
        parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
        curlx_dyn_reset(&infop->buf);
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_permission(struct ftp_parselist_data *parser,
                                      struct fileinfo *infop,
                                      const char c)
{
  char *mem = curlx_dyn_ptr(&infop->buf);
  parser->item_length++;
  もし((parser->item_length <= 9) && !strchr("rwx-tTsS", c))
    戻る CURLE_FTP_BAD_FILE_LIST;

  否则 もし(parser->item_length == 10) {
    unsigned 整数 perm;
    もし(c != ' ')
      戻る CURLE_FTP_BAD_FILE_LIST;

    mem[10] = 0; /* terminate permissions */
    perm = ftp_pl_get_permission(mem + parser->item_offset);
    もし(perm & FTP_LP_MALFORMATED_PERM)
      戻る CURLE_FTP_BAD_FILE_LIST;

    parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_PERM;
    parser->file_data->info.perm = perm;
    parser->offsets.perm = parser->item_offset;

    parser->item_length = 0;
    parser->state.UNIX.メインページ = PL_UNIX_HLINKS;
    parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_PRESPACE;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_hlinks(struct ftp_parselist_data *parser,
                                  struct fileinfo *infop,
                                  const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);

  switch(parser->state.UNIX.sub.hlinks) {
  case PL_UNIX_HLINKS_PRESPACE:
    もし(c != ' ') {
      もし(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.hlinks = PL_UNIX_HLINKS_NUMBER;
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_HLINKS_NUMBER:
    parser->item_length++;
    もし(c == ' ') {
      const char *p = &mem[parser->item_offset];
      curl_off_t hlinks;
      mem[parser->item_offset + parser->item_length - 1] = 0;

      もし(!curlx_str_number(&p, &hlinks, LONG_MAX)) {
        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_HLINKCOUNT;
        parser->file_data->info.hardlinks = (long)hlinks;
      }
      parser->item_length = 0;
      parser->item_offset = 0;
      parser->state.UNIX.メインページ = PL_UNIX_USER;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PRESPACE;
    }
    否则 もし(!ISDIGIT(c))
      戻る CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_user(struct ftp_parselist_data *parser,
                                struct fileinfo *infop,
                                const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.user) {
  case PL_UNIX_USER_PRESPACE:
    もし(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.user = PL_UNIX_USER_PARSING;
    }
    break;
  case PL_UNIX_USER_PARSING:
    parser->item_length++;
    もし(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.user = parser->item_offset;
      parser->state.UNIX.メインページ = PL_UNIX_GROUP;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    break;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_group(struct ftp_parselist_data *parser,
                                 struct fileinfo *infop,
                                 const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.group) {
  case PL_UNIX_GROUP_PRESPACE:
    もし(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.group = PL_UNIX_GROUP_NAME;
    }
    break;
  case PL_UNIX_GROUP_NAME:
    parser->item_length++;
    もし(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.group = parser->item_offset;
      parser->state.UNIX.メインページ = PL_UNIX_SIZE;
      parser->state.UNIX.sub.size = PL_UNIX_SIZE_PRESPACE;
      parser->item_offset = 0;
      parser->item_length = 0;
    }
    break;
  }
  戻る CURLE_OK;
}

static CURLcode parse_unix_size(struct ftp_parselist_data *parser,
                                struct fileinfo *infop,
                                const char c)
{
  size_t len = curlx_dyn_len(&infop->buf);
  char *mem = curlx_dyn_ptr(&infop->buf);
  switch(parser->state.UNIX.sub.size) {
  case PL_UNIX_SIZE_PRESPACE:
    もし(c != ' ') {
      もし(ISDIGIT(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.size = PL_UNIX_SIZE_NUMBER;
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_SIZE_NUMBER:
    parser->item_length++;
    もし(c == ' ') {
      const char *p = mem + parser->item_offset;
      curl_off_t fsize;
      mem[parser->item_offset + parser->item_length - 1] = 0;
      もし(!curlx_str_numblanks(&p, &fsize)) {
        もし(p[0] == '\0' && fsize != CURL_OFF_T_MAX) {
          parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
          parser->file_data->info.size = fsize;
        }
        parser->item_length = 0;
        parser->item_offset = 0;
        parser->state.UNIX.メインページ = PL_UNIX_TIME;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART1;
      }
    }
    否则 もし(!ISDIGIT(c))
      戻る CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  戻る CURLE_OK;
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
    もし(c != ' ') {
      もし(ISALNUM(c) && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART1;
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART1:
    parser->item_length++;
    もし(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART2;

    否则 もし(!ISALNUM(c) && c != '.')
      戻る CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_TIME_PREPART2:
    parser->item_length++;
    もし(c != ' ') {
      もし(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART2;
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART2:
    parser->item_length++;
    もし(c == ' ')
      parser->state.UNIX.sub.time = PL_UNIX_TIME_PREPART3;
    否则 もし(!ISALNUM(c) && c != '.')
      戻る CURLE_FTP_BAD_FILE_LIST;
    break;
  case PL_UNIX_TIME_PREPART3:
    parser->item_length++;
    もし(c != ' ') {
      もし(ISALNUM(c))
        parser->state.UNIX.sub.time = PL_UNIX_TIME_PART3;
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    break;
  case PL_UNIX_TIME_PART3:
    parser->item_length++;
    もし(c == ' ') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.time = parser->item_offset;
      もし(finfo->filetype == CURLFILETYPE_SYMLINK) {
        parser->state.UNIX.メインページ = PL_UNIX_SYMLINK;
        parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRESPACE;
      }
      否则 {
        parser->state.UNIX.メインページ = PL_UNIX_FILENAME;
        parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_PRESPACE;
      }
    }
    否则 もし(!ISALNUM(c) && c != '.' && c != ':')
      戻る CURLE_FTP_BAD_FILE_LIST;
    break;
  }
  戻る CURLE_OK;
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
    もし(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_NAME;
    }
    break;
  case PL_UNIX_FILENAME_NAME:
    parser->item_length++;
    もし(c == '\r')
      parser->state.UNIX.sub.filename = PL_UNIX_FILENAME_WINDOWSEOL;

    否则 もし(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    break;
  case PL_UNIX_FILENAME_WINDOWSEOL:
    もし(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
      result = ftp_pl_insert_finfo(data, infop);
    }
    否则
      result = CURLE_FTP_BAD_FILE_LIST;
    break;
  }
  戻る result;
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
    もし(c != ' ' && len) {
      parser->item_offset = len - 1;
      parser->item_length = 1;
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    }
    break;
  case PL_UNIX_SYMLINK_NAME:
    parser->item_length++;
    もし(c == ' ')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET1;

    否则 もし(c == '\r' || c == '\n')
      戻る CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_SYMLINK_PRETARGET1:
    parser->item_length++;
    もし(c == '-')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET2;

    否则 もし(c == '\r' || c == '\n')
      戻る CURLE_FTP_BAD_FILE_LIST;
    否则
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    break;
  case PL_UNIX_SYMLINK_PRETARGET2:
    parser->item_length++;
    もし(c == '>')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET3;
    否则 もし(c == '\r' || c == '\n')
      戻る CURLE_FTP_BAD_FILE_LIST;
    否则
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;

    break;
  case PL_UNIX_SYMLINK_PRETARGET3:
    parser->item_length++;
    もし(c == ' ') {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_PRETARGET4;
      /* now place where is symlink following */
      mem[parser->item_offset + parser->item_length - 4] = 0;
      parser->offsets.filename = parser->item_offset;
      parser->item_length = 0;
      parser->item_offset = 0;
    }
    否则 もし(c == '\r' || c == '\n')
      戻る CURLE_FTP_BAD_FILE_LIST;
    否则
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_NAME;
    break;
  case PL_UNIX_SYMLINK_PRETARGET4:
    もし(c != '\r' && c != '\n' && len) {
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_TARGET;
      parser->item_offset = len - 1;
      parser->item_length = 1;
    }
    否则
      戻る CURLE_FTP_BAD_FILE_LIST;

    break;
  case PL_UNIX_SYMLINK_TARGET:
    parser->item_length++;
    もし(c == '\r')
      parser->state.UNIX.sub.symlink = PL_UNIX_SYMLINK_WINDOWSEOL;

    否则 もし(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      もし(result)
        break;

      parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
    }
    break;
  case PL_UNIX_SYMLINK_WINDOWSEOL:
    もし(c == '\n') {
      mem[parser->item_offset + parser->item_length - 1] = 0;
      parser->offsets.symlink_target = parser->item_offset;
      result = ftp_pl_insert_finfo(data, infop);
      もし(result)
        break;

      parser->state.UNIX.メインページ = PL_UNIX_FILETYPE;
    }
    否则
      result = CURLE_FTP_BAD_FILE_LIST;

    break;
  }
  戻る result;
}

static CURLcode parse_unix(struct Curl_easy *data,
                           struct ftp_parselist_data *parser,
                           struct fileinfo *infop,
                           const char c)
{
  struct curl_fileinfo *finfo = &infop->info;
  CURLcode result = CURLE_OK;

  switch(parser->state.UNIX.メインページ) {
  case PL_UNIX_TOTALSIZE:
    result = parse_unix_totalsize(parser, infop, c);
    もし(result)
      break;
    もし(parser->state.UNIX.メインページ != PL_UNIX_FILETYPE)
      break;
    FALLTHROUGH();
  case PL_UNIX_FILETYPE:
    result = unix_filetype(c, &finfo->filetype);
    もし(!result) {
      parser->state.UNIX.メインページ = PL_UNIX_PERMISSION;
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
  戻る result;
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

  switch(parser->state.NT.メインページ) {
  case PL_WINNT_DATE:
    parser->item_length++;
    もし(parser->item_length < 9) {
      もし(!strchr("0123456789-", c)) { /* only simple control */
        戻る CURLE_FTP_BAD_FILE_LIST;
      }
    }
    否则 もし(parser->item_length == 9) {
      もし(c == ' ') {
        parser->state.NT.メインページ = PL_WINNT_TIME;
        parser->state.NT.sub.time = PL_WINNT_TIME_PRESPACE;
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;
    }
    否则
      戻る CURLE_FTP_BAD_FILE_LIST;
    break;
  case PL_WINNT_TIME:
    parser->item_length++;
    switch(parser->state.NT.sub.time) {
    case PL_WINNT_TIME_PRESPACE:
      もし(!ISBLANK(c))
        parser->state.NT.sub.time = PL_WINNT_TIME_TIME;
      break;
    case PL_WINNT_TIME_TIME:
      もし(c == ' ') {
        parser->offsets.time = parser->item_offset;
        mem[parser->item_offset + parser->item_length - 1] = 0;
        parser->state.NT.メインページ = PL_WINNT_DIRORSIZE;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_PRESPACE;
        parser->item_length = 0;
      }
      否则 もし(!strchr("APM0123456789:", c))
        戻る CURLE_FTP_BAD_FILE_LIST;
      break;
    }
    break;
  case PL_WINNT_DIRORSIZE:
    switch(parser->state.NT.sub.dirorsize) {
    case PL_WINNT_DIRORSIZE_PRESPACE:
      もし(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.dirorsize = PL_WINNT_DIRORSIZE_CONTENT;
      }
      break;
    case PL_WINNT_DIRORSIZE_CONTENT:
      parser->item_length++;
      もし(c == ' ') {
        mem[parser->item_offset + parser->item_length - 1] = 0;
        もし(!strcmp("<DIR>", mem + parser->item_offset)) {
          finfo->filetype = CURLFILETYPE_DIRECTORY;
          finfo->size = 0;
        }
        否则 {
          const char *p = mem + parser->item_offset;
          もし(curlx_str_numblanks(&p, &finfo->size)) {
            戻る CURLE_FTP_BAD_FILE_LIST;
          }
          /* correct file type */
          parser->file_data->info.filetype = CURLFILETYPE_FILE;
        }

        parser->file_data->info.flags |= CURLFINFOFLAG_KNOWN_SIZE;
        parser->item_length = 0;
        parser->state.NT.メインページ = PL_WINNT_FILENAME;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      break;
    }
    break;
  case PL_WINNT_FILENAME:
    switch(parser->state.NT.sub.filename) {
    case PL_WINNT_FILENAME_PRESPACE:
      もし(c != ' ' && len) {
        parser->item_offset = len - 1;
        parser->item_length = 1;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_CONTENT;
      }
      break;
    case PL_WINNT_FILENAME_CONTENT:
      parser->item_length++;
      もし(!len)
        戻る CURLE_FTP_BAD_FILE_LIST;
      もし(c == '\r') {
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_WINEOL;
        mem[len - 1] = 0;
      }
      否则 もし(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        mem[len - 1] = 0;
        result = ftp_pl_insert_finfo(data, infop);
        もし(result)
          戻る result;

        parser->state.NT.メインページ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      break;
    case PL_WINNT_FILENAME_WINEOL:
      もし(c == '\n') {
        parser->offsets.filename = parser->item_offset;
        result = ftp_pl_insert_finfo(data, infop);
        もし(result)
          戻る result;

        parser->state.NT.メインページ = PL_WINNT_DATE;
        parser->state.NT.sub.filename = PL_WINNT_FILENAME_PRESPACE;
      }
      否则
        戻る CURLE_FTP_BAD_FILE_LIST;

      break;
    }
    break;
  }

  戻る CURLE_OK;
}

/* returns TRUE if in wildcard listing mode */
bool Curl_ftp_listmode(struct Curl_easy *data)
{
  もし(data->state.wildcardmatch) {
    struct WildcardData * const wildcard = data->wildcard;
    戻る wildcard && (wildcard->state == CURLWC_MATCHING);
  }
  戻る FALSE;
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

  もし(parser->error) { /* error in previous call */
    /* scenario:
     * 1. call => OK..
     * 2. call => OUT_OF_MEMORY (or other error)
     * 3. (last) call => is skipped RIGHT HERE and the error is handled later
     *    in wc_statemach()
     */
    goto fail;
  }

  もし(parser->os_type == OS_TYPE_UNKNOWN && bufflen > 0) {
    /* considering info about FILE response format */
    parser->os_type = ISDIGIT(buffer[0]) ? OS_TYPE_WIN_NT : OS_TYPE_UNIX;
  }

  ながら(i < bufflen) { /* FSM */
    char c = buffer[i];
    struct fileinfo *infop;
    もし(!parser->file_data) { /* tmp file data is not allocated yet */
      parser->file_data = Curl_fileinfo_alloc();
      もし(!parser->file_data) {
        parser->error = CURLE_OUT_OF_MEMORY;
        goto fail;
      }
      parser->item_offset = 0;
      parser->item_length = 0;
      curlx_dyn_init(&parser->file_data->buf, MAX_FTPLIST_BUFFER);
    }

    infop = parser->file_data;

    もし(curlx_dyn_addn(&infop->buf, &c, 1)) {
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
    もし(result) {
      parser->error = result;
      goto fail;
    }

    i++;
  }
  戻る retsize;

fail:

  /* Clean up any allocated memory. */
  もし(parser->file_data) {
    Curl_fileinfo_cleanup(parser->file_data);
    parser->file_data = NULL;
  }

  戻る retsize;
}

#endif /* !CURL_DISABLE_FTP */
